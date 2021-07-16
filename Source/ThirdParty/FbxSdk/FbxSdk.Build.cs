using System;
using System.IO;
using UnrealBuildTool;

public class FbxSdk : ModuleRules
{


    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(ModuleDirectory); }
    }

    private string RuntimeThirdPartyPath
    {
        get { return "$(PluginDir)/Source/ThirdParty/FbxSdk"; }
    }


    public FbxSdk(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "include"));

        string LibPath;
        if (Target.Platform == UnrealTargetPlatform.Win32)
            LibPath = Path.Combine(ThirdPartyPath, "lib", "X86", "libfbxsdk.lib");
        else
            LibPath = Path.Combine(ThirdPartyPath, "lib", "X64", "libfbxsdk.lib");

        PublicAdditionalLibraries.Add(LibPath);


        string DllPath;
        if (Target.Platform == UnrealTargetPlatform.Win32)
            DllPath = Path.Combine(RuntimeThirdPartyPath, "lib", "X86", "libfbxsdk.dll");
        else
            DllPath = Path.Combine(RuntimeThirdPartyPath, "lib", "X64", "libfbxsdk.dll");


        PublicDelayLoadDLLs.Add("libfbxsdk.dll");
        RuntimeDependencies.Add(new RuntimeDependency(DllPath));
    }
}