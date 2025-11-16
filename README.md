# IL2CPP DirectX ImGui Base

A comprehensive C++ DLL injection framework for Unity IL2CPP games featuring DirectX 11 hooking and ImGui integration with custom UI components.

## 🎯 Overview

This project provides a robust base for creating game modifications and cheats for Unity games compiled with IL2CPP. It includes a complete DirectX 11 rendering pipeline, IL2CPP resolver, and a modern ImGui interface with custom animated components.

## ✨ Features

- **DirectX 11 Hooking**: Complete DirectX 11 Present hook implementation for rendering overlays
- **IL2CPP Resolver**: Full IL2CPP runtime resolver with support for:
  - Class and method resolution
  - String manipulation
  - Thread management
  - Domain access
  - Unity API wrappers (GameObject, Transform, Camera, Rigidbody, etc.)
- **ImGui Integration**: Modern UI framework with custom components:
  - Animated checkboxes with smooth transitions
  - Custom styling and theming support
  - Full ImGui demo and widgets
- **Obfuscation Support**: Built-in support for obfuscated Unity exports (ROT, etc.)
- **Memory Management**: RAII-based resource management with smart pointers
- **Modular Architecture**: Clean separation of concerns with organized folder structure

## 📁 Project Structure

```
IL2CPP-DirectX-ImGui-Base/
├── Cheat/
│   ├── Functions/        # Game-specific cheat functions
│   ├── Hooks/           # Hook implementations
│   ├── Signatures/      # Pattern scanning and signatures
│   └── Utils/           # Offsets, SDK, and variables
├── IL2CPPResolver/
│   ├── Functions/       # IL2CPP function wrappers
│   ├── Unity/          # Unity API implementations
│   │   ├── API/        # Unity component wrappers
│   │   └── Structures/ # IL2CPP data structures
│   └── Utils/          # Helper utilities
├── Utils/
│   ├── Backend/        # DirectX backend and rendering
│   ├── ImGui/          # ImGui library files
│   ├── Math/           # Vector math utilities
│   └── Utils.h         # Common utilities and includes
└── DllMain.cpp         # Entry point
```

## 🛠️ Requirements

- **Visual Studio 2022** (v143 platform toolset)
- **Windows SDK 10.0**
- **C++17** or later (C++20 recommended for Release builds)
- **vcpkg** (for dependency management)
- **DirectX 11 SDK**

## 🚀 Building

1. **Clone the repository**
   ```bash
   git clone (https://github.com/Escarabajoz/IL2CPP-DirectX-ImGui.git
   cd IL2CPP-DirectX-ImGui-Base
   ```

2. **Open the solution**
   ```
   Open DX11Hook.sln in Visual Studio 2022
   ```

3. **Configure build settings**
   - Select **Release** configuration
   - Select **x64** platform
   - Ensure vcpkg integration is enabled

4. **Build the project**
   - Build → Build Solution (Ctrl+Shift+B)
   - Output DLL will be in `Build/x64/`

## 📦 Usage

### Basic Injection

1. Build the project in **Release | x64** configuration
2. Use your preferred DLL injector to inject the built DLL into the target Unity IL2CPP game
3. Press **END** key to uninject and cleanup

### Customization

#### Adding Custom Cheats

Edit `Cheat/Functions/Functions.h` to add your custom cheat functions:

```cpp
namespace CheatFunctions
{
    void YourCustomFunction();
}
```

#### Modifying the UI

The main UI rendering is handled in `Utils/Backend/Backend.cpp` in the `RenderCheat()` method:

```cpp
void Backend::RenderCheat()
{
    ImGui::Begin("Your Cheat Menu");
    
    // Use custom animated checkbox
    static bool feature = false;
    AnimatedCheckbox("Feature Name", &feature);
    
    ImGui::End();
}
```

#### IL2CPP Resolution

Initialize IL2CPP resolver in your hooks:

```cpp
IL2CPP::Initialize(true); // Wait for GameAssembly.dll

// Resolve classes and methods
auto* klass = IL2CPP::Class::Find("Assembly-CSharp", "Namespace.ClassName");
auto* method = IL2CPP::Class::Utils::GetMethodPointer(klass, "MethodName");
```

## 🎨 Custom UI Components

### Animated Checkbox

The project includes a custom animated checkbox with smooth transitions:

```cpp
static bool myFeature = false;
AnimatedCheckbox("My Feature", &myFeature);
```

Features:
- Smooth color interpolation
- Animated checkmark drawing
- Hover effects
- Modern styling

## 🔧 Configuration

### Compiler Settings (Release | x64)

- **Language Standard**: C++20 (stdcpplatest)
- **Disabled Warnings**: 4244, 4101, 4042, 4305
- **Optimizations**: Full optimization enabled
- **Static Linking**: vcpkg static libraries

### DirectX Hook

The DirectX 11 Present hook is automatically initialized on DLL injection. The hook intercepts the swap chain's Present call to render the ImGui overlay.

## 🔒 Security Considerations

- Uses lazy imports to avoid detection
- RAII-based resource management prevents memory leaks
- Proper cleanup on uninject (END key)
- No console window in Release builds

## 📝 Key Files

- **DllMain.cpp**: Entry point and main thread
- **Utils/Backend/Backend.cpp**: DirectX hooking and ImGui rendering
- **Cheat/Hooks/Hooks.cpp**: Hook initialization and management
- **IL2CPPResolver/il2cpp_resolver.cpp**: IL2CPP runtime resolver

## 🐛 Debugging

For debugging purposes:
1. Switch to **Debug | x64** configuration
2. Console window will be created automatically
3. Use `printf` or logging to track execution

## ⚠️ Disclaimer

This project is for **educational purposes only**. The authors are not responsible for any misuse of this software. Using this software to cheat in online games may violate terms of service and result in bans.

## 📄 License

This project is provided as-is without any warranty. Use at your own risk.

## 🤝 Contributing

Contributions are welcome! Please ensure your code follows the existing style:
- PascalCase for classes
- camelCase for variables and methods
- SCREAMING_SNAKE_CASE for constants
- Prefix member variables with `m_`

## 🔗 Dependencies

- **ImGui**: Immediate mode GUI library
- **DirectX 11**: Graphics API
- **MinHook**: Function hooking library (referenced in project)
- **IL2CPP**: Unity's IL2CPP runtime

## 📚 Resources

- [ImGui Documentation](https://github.com/ocornut/imgui)
- [IL2CPP Internals](https://docs.unity3d.com/Manual/IL2CPP.html)
- [DirectX 11 Documentation](https://docs.microsoft.com/en-us/windows/win32/direct3d11/atoc-dx-graphics-direct3d-11)

---

**Note**: Always test in a safe environment before using on live games. Respect game developers and other players.


**Thanks MEGALLM**: Thank you for the credits that helped me move the project forward.
