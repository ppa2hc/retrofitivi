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
    pFunc = PyObject_GetAttrString(pModule, "getVssTargetValue");
}

VssThread::VssThread(AiassistAsync *parent)
{
    m_parent = parent;
}

void VssThread::run()
{
    QString oldText;

    initPyEnv();
    
    while(1) {
        //qDebug() << "VssThread::run";
        QString value;

        value = getVssApiValue("Vehicle.TextToSpeech");

        if (value != "nullstring") {
            if (oldText != value) {
                m_parent->setTextToSpeech(value);
                oldText = value;
            }            
        }

        QThread::msleep(100);
    }
}

QString VssThread::getVssApiValue(QString apiName)
{
    if (pModule != nullptr) {

        // Check if the function is callable
        if (PyCallable_Check(pFunc)) {
            // Call the function
            PyObject *pValue = PyObject_CallObject(pFunc, nullptr);
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