#ifndef PACKETTRANSFORMER_H
#define PACKETTRANSFORMER_H

#include <QByteArray>
#include "def.h"
#include "serialPacket.h"

#define MAX_BUFFER_LENGTH	1024

class PacketTransformer
{
public:
    DSP_INST_PACKET *dspInst;
public:
    PacketTransformer();
    PacketTransformer(PBYTE buf,int len);
    void SetRawPacket(PBYTE buf,int len);
    void SetRawPacket(QByteArray &recv);
    void Reset();
    void AppandArray(QByteArray &recv);

    bool ConstructPacket();
    bool DestructPacket();
    bool TryDestructOnePacket();
    QByteArray& GetByteArray();
    BYTE CheckSum();
    int GetLength()
    {
        return length;
    };

protected:
    QByteArray result;
    int length;
    BYTE buffer[MAX_BUFFER_LENGTH];
};

#endif // PACKETTRANSFORMER_H
