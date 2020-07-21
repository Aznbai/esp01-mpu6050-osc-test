void sendRawGyro()
{ // send Rotation
    OSCMessage msg("/1/rawGyroXYZ");
    msg.add(GyX);
    msg.add(GyY);
    msg.add(GyZ);
    Udp.beginPacket(targetIp, targetPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}
void sendRawAccel()
{ // send the calibrated gForce
    OSCMessage msg("/1/rawAccXYZ");
    msg.add(AcX);
    msg.add(AcY);
    msg.add(AcZ);
    Udp.beginPacket(targetIp, targetPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}
void sendRawTemp()
{ // send the calibrated gForce
    OSCMessage msg("/1/rawTemp");
    msg.add(Tmp);
    Udp.beginPacket(targetIp, targetPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}

void sendNormalGyro()
{ // send Rotation
    OSCMessage msg("/1/normalGyroXYZ");
    msg.add(GyXNormal);
    msg.add(GyYNormal);
    msg.add(GyZNormal);
    Udp.beginPacket(targetIp, targetPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}
void sendNormalAccel()
{ // send the calibrated gForce
    OSCMessage msg("/1/normalAccXYZ");
    msg.add(AcXNormal);
    msg.add(AcYNormal);
    msg.add(AcZNormal);
    Udp.beginPacket(targetIp, targetPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}
void sendNormalTemp()
{ // send the calibrated gForce
    OSCMessage msg("/1/normalTemp");
    msg.add(TmpNormal);
    Udp.beginPacket(targetIp, targetPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}

void sendRPY()
{
    OSCMessage msg("/1/RPY");
    msg.add(roll);
    msg.add(pitch);
    msg.add(yaw);
    Udp.beginPacket(targetIp, targetPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}
void sendStatusString(const char *txt)
{
    OSCMessage msg("/status");
    msg.add(txt);
    Udp.beginPacket(targetIp, targetPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
}