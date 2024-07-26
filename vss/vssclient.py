from kuksa_client.grpc import VSSClient

DIGITAL_AUTO_IP = '127.0.0.1'

def getCurrentValue(vapi):
    with VSSClient(DIGITAL_AUTO_IP, 55555) as client:
        current_values = client.get_current_values([vapi])
        if current_values[vapi] is not None:
            # print(current_values[vapi].value)
            return current_values[vapi].value
        else:
            return "nullstring"  # Ensure a string is always returned

def getVssCurrentValue(vapi):
    return getCurrentValue(vapi)

def getTargetValue(vapi):
    with VSSClient(DIGITAL_AUTO_IP, 55555) as client:
        current_values = client.get_target_values([vapi])
        if current_values[vapi] is not None:
            # print(current_values[vapi].value)
            return current_values[vapi].value
        else:
            return "nullstring"  # Ensure a string is always returned

def getVssTargetValue(vapi):
    return getTargetValue(vapi)

# def getVssTargetValue():
#     return getTargetValue("Vehicle.TextToSpeech")

# getVssCurrentValue("Vehicle.TextToSpeech")
# getVssTargetValue("Vehicle.TextToSpeech")
