#include "Signatures.h"

bool Signatures::SearchSignatures(bool NeedDebug)
{
    Unity::il2cppClass* GetUnityCamera = IL2CPP::Class::Find("UnityEngine.Camera");
    
    if (GetUnityCamera != nullptr)
    {
        Offsets::GetCameraMainOffset = (uintptr_t)IL2CPP::Class::Utils::GetMethodPointer(GetUnityCamera, "get_main");
        
        if(NeedDebug && Offsets::GetCameraMainOffset != 0)
            Utils::AddressLog(Offsets::GetCameraMainOffset - UnitySDK::UnityGameAssembly, "GetUnityCamera");
        else if(NeedDebug)
            printf("[ @zzzmate ] WARNING: GetCameraMainOffset is NULL\n");
    }
    else
    {
        printf("[ @zzzmate ] ERROR: UnityEngine.Camera class not found!\n");
        return false;
    }

    return true;
}
