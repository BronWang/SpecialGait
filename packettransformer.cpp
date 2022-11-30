#include "PacketTransformer.h"
#include "serialPacket.h"
#include <QDebug>

PacketTransformer::PacketTransformer()
{
    dspInst=(DSP_INST_PACKET *)buffer;
    length=0;
}

PacketTransformer::PacketTransformer(PBYTE buf,int len)
{
    dspInst=(DSP_INST_PACKET *)buffer;
    memcpy(PBYTE(&buffer),buf,length);
    length=len;
}

void PacketTransformer::SetRawPacket(PBYTE buf,int len)
{
    result.clear();
    memcpy(PBYTE(&buffer),buf,length);
    length=len;
}

void PacketTransformer::SetRawPacket(QByteArray &recv)
{
    result.clear();
    result.setRawData(recv, recv.length());
}

QByteArray& PacketTransformer::GetByteArray()
{
    return result;
}

bool PacketTransformer::ConstructPacket()
{
    int i=0;

    result.clear();
    result.resize(dspInst->length+4);
    result[0] = 0xff;
    result[1] = 0xff;
    for(i=0;i<dspInst->length+1;i++)
        result[i+2] = buffer[i];
    result[i+2] = CheckSum();
    return true;

}

bool PacketTransformer::DestructPacket()
{
    PBYTE pbuf=NULL;
    if(result.size()<6){

        return false;
    }
    pbuf=reinterpret_cast<PBYTE>(result.data());
    if(
        0xff!=pbuf[0]||
        0xff!=pbuf[1]
        ){
        qDebug()<<"DestructPacket " << QString::number(pbuf[0],16) << QString::number(pbuf[1],16);
        return false;
    }
    memcpy(buffer,pbuf+2,result.size()-3);
    if(
        dspInst->id!=ID_DSP||
        dspInst->length!=result.size()-4||
        pbuf[result.size()-1]!=CheckSum()
        )
        return false;
    return true;
}

bool PacketTransformer::TryDestructOnePacket()
{
    PBYTE pbuf=NULL;
    DSP_INST_PACKET *ppacket=NULL;

    while(result.size()>=6)
    {
        pbuf=reinterpret_cast<PBYTE>(result.data());
        if(pbuf[0]!=0xff)
        {
            result.remove(0,1);
            continue;
        }
        if(pbuf[1]!=0xff)
        {
            result.remove(0,2);
            continue;
        }
        ppacket=(DSP_INST_PACKET *)(pbuf+2);
        if(ppacket->id!=ID_DSP)
        {
            result.remove(0,3);
            continue;
        }
        if(result.size()<ppacket->length+4)
            return false;
        memcpy(buffer,ppacket,ppacket->length+2);
        result.remove(0,dspInst->length+4);
        if(buffer[ppacket->length+1]!=CheckSum())
            continue;
        return true;
    }
    return false;
}

BYTE PacketTransformer::CheckSum()
{
    BYTE checkSum=0;
    for(int i=0;i<dspInst->length+1;i++)
        checkSum+=buffer[i];
    checkSum=~checkSum;
    return checkSum;
}

void PacketTransformer::Reset()
{
    result.clear();
    result.resize(0);
}

void PacketTransformer::AppandArray(QByteArray &recv)
{
    result.append(recv);
}
