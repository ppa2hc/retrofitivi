#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QFileInfo>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

#include "aiassist.hpp"
#include <iostream>
#include <string>

// Python global object
PyObject *pFunc;
PyObject *pName;
PyObject *pModule;

void initPyEnv()
{
    // Initialize the Python Interpreter
    Py_Initialize();

    // Set the Python script's directory (optional)
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")"); // Add the current directory to the Python path

    // Load the script
    pName = PyUnicode_DecodeFSDefault("vssclient");
    pModule = PyImport_Import(pName);
    pFunc = PyObject_GetAttrString(pModule, "getCurrentValue");
}

VssThread::VssThread(AiassistAsync *parent)
{
    m_parent = parent;
}

void VssThread::run()
{    
    QString oldTimeStamp;
    QString oldText;

    initPyEnv();
    
    while(1) {
        //qDebug() << "VssThread::run";
        QString value;
        QString currentTimeStamp;

        value = getVssApiValue("Vehicle.TextToSpeech", currentTimeStamp);
        // qDebug() << oldTimeStamp <<  " -------- " << currentTimeStamp;

        if (value != "nullstring") {
            if (currentTimeStamp != oldTimeStamp) {
                m_parent->setTextToSpeech(value);
                oldTimeStamp = currentTimeStamp;
                oldText = value;
            }            
        }

        QThread::msleep(1000);
    }
}

QString VssThread::getVssApiValue(QString apiName, QString &currentTimeStamp)
{
    if (pModule != nullptr) {

        // Check if the function is callable
        if (PyCallable_Check(pFunc)) {

            // Prepare the arguments for the function call
            PyObject *pArgs = PyTuple_Pack(1, PyUnicode_FromString(apiName.toStdString().c_str()));

            // Call the function
            PyObject *pValue = PyObject_CallObject(pFunc, pArgs);            
            Py_DECREF(pArgs);
            // Check if the result is a tuple
            if (PyTuple_Check(pValue)) {
                PyObject *pVal = PyTuple_GetItem(pValue, 0);
                PyObject *pTime = PyTuple_GetItem(pValue, 1);
                // qDebug() << "PyTuple_Check True";

                if (pVal && pTime) {
                    const char *resultCStr = PyUnicode_AsUTF8(pVal);
                    const char *timestampCStr = PyUnicode_AsUTF8(pTime);

                    if (resultCStr && timestampCStr) {
                        std::string result = resultCStr;
                        std::string timestamp = timestampCStr;
                        currentTimeStamp = QString::fromStdString(timestamp);
                        Py_DECREF(pValue);
                        // return std::make_tuple(QString::fromStdString(result), QString::fromStdString(timestamp));
                        return QString::fromStdString(result);
                    }
                 }
            }
            else {
                // qDebug() << "PyTuple_Check False";
                if (pValue != nullptr) {
                    // Convert the result to a C++ string
                    const char* resultCStr = PyUnicode_AsUTF8(pValue);
                    if (resultCStr != nullptr) {
                        std::string result = resultCStr;
                        Py_DECREF(pValue);
                        return QString::fromStdString(result);
                    } else {
                        Py_DECREF(pValue);
                        PyErr_Print();
                        std::cerr << "Error: PyUnicode_AsUTF8 returned null" << std::endl;
                    }
                } else {
                    PyErr_Print();
                    std::cerr << "Call to get_tts_text failed" << std::endl;
                }
            }            
        } else {
            if (PyErr_Occurred()) PyErr_Print();
            std::cerr << "Cannot find function 'get_tts_text'" << std::endl;
        }        
    } else {
        PyErr_Print();
        std::cerr << "Failed to load 'vssclient'" << std::endl;
    } 

    return "nullstring";
}

AiassistAsync::AiassistAsync()
{
    vssThread = new VssThread(this);    
    vssThread->start();    
}

AiassistAsync::~AiassistAsync()
{
    if(vssThread)
        delete vssThread;
    Py_Finalize();
}

void AiassistAsync::setTextToSpeech(QString msg)
{
    updateTextToSpeech(msg);
}