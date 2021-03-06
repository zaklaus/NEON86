#include "StdAfx.h"
#include "SoundBase.h"

void CSoundBase::SetVolume(LONG vol)
{
    double attenuation = 1.0 / 1024.0 + vol / 100.0 * 1023.0 / 1024.0;
    double db = 10 * log10(attenuation) / log10(2);
    LONG realVol = static_cast<LONG>(db * 100);
    mBuffer->SetVolume(realVol);
}

LONG CSoundBase::GetVolume()
{
    LONG vol;
    mBuffer->GetVolume(&vol);
    float actVol = powf(10, (static_cast<float>(vol) / 100.0f) * log10f(2) / 10.0f) * 100.0f;
    return static_cast<LONG>(floor(actVol));
}

void CSoundBase::Play()
{
    mBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

void CSoundBase::Pause()
{
    mBuffer->Stop();
}

void CSoundBase::Stop()
{
    Pause();
    mBuffer->SetCurrentPosition(0);
}

void CSoundBase::SetPan(LONG pan)
{
    LONG realPan = static_cast<LONG>(ScaleBetween(static_cast<float>(pan), DSBPAN_LEFT, DSBPAN_RIGHT, -100, 100));
    mBuffer->SetPan(realPan);
}

LONG CSoundBase::GetPan()
{
    LONG pan;
    mBuffer->GetPan(&pan);
    return static_cast<LONG>(ScaleBetween(static_cast<float>(pan), -100, 100, DSBPAN_LEFT, DSBPAN_RIGHT));
}

DWORD CSoundBase::GetCurrentPosition()
{
    DWORD playPos = 0x000000;
    mBuffer->GetCurrentPosition(&playPos, nullptr);
    return playPos;
}

void CSoundBase::SetCurrentPosition(DWORD cursor)
{
    mBuffer->SetCurrentPosition(cursor);
}

bool CSoundBase::IsPlaying()
{
    DWORD playStatus = 0x000000;
    mBuffer->GetStatus(&playStatus);
    return playStatus & DSBSTATUS_PLAYING;
}

DWORD CSoundBase::GetTotalSize()
{
    DSBCAPS caps;
    mBuffer->GetCaps(&caps);
    return caps.dwBufferBytes;
}
