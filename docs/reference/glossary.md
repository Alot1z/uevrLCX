# UEVR Glossary

This glossary defines technical terms, acronyms, and concepts used throughout the UEVR documentation. Use this as a reference when reading other documentation sections.

## A

### **Adapter**
A software component that enables UEVR to work with a specific game engine or rendering system. Adapters translate UEVR's VR functionality into engine-specific implementations.

### **AFR (Alternate Frame Rendering)**
A rendering technique where the left and right eye frames are rendered on alternating frames, reducing GPU load while maintaining VR performance.

### **API (Application Programming Interface)**
A set of functions, protocols, and tools that allow different software applications to communicate and interact with each other.

### **Anti-aliasing**
A technique used to smooth jagged edges in rendered graphics, improving visual quality in VR applications.

### **AppImage**
A Linux application packaging format that includes all dependencies, making it easy to distribute and run applications across different Linux distributions.

## B

### **Backbuffer**
A memory buffer that holds the rendered frame before it's displayed on screen. In VR, multiple backbuffers are used for left and right eye rendering.

### **Beta Release**
A pre-release version of software made available for testing and feedback before the final release.

### **Bindings**
The mapping between VR controller inputs and game actions, allowing users to customize how their VR controllers interact with games.

### **Blue-Green Deployment**
A deployment strategy that maintains two identical production environments, allowing for zero-downtime updates by switching traffic between them.

### **Build System**
The collection of tools and processes used to compile, link, and package UEVR from source code into executable binaries.

## C

### **Canary Deployment**
A deployment strategy that releases new features to a small subset of users first, allowing for early detection of issues before full rollout.

### **CMake**
A cross-platform build system generator that uses configuration files to generate native build files for various platforms and compilers.

### **Code Injection**
A technique where UEVR inserts its own code into a running game process to enable VR functionality.

### **Code Signing**
The process of digitally signing software to verify its authenticity and integrity, ensuring it hasn't been tampered with.

### **Configuration File**
A file that stores UEVR settings and preferences, allowing users to customize their VR experience without modifying code.

### **Container**
A lightweight, isolated environment that packages an application and its dependencies, ensuring consistent behavior across different systems.

### **Controller Mapping**
The configuration that defines how VR controller inputs are translated into game actions and movements.

### **Cross-Platform**
Software that can run on multiple operating systems and platforms without modification.

## D

### **D3D (Direct3D)**
Microsoft's graphics API for Windows, used by UEVR to render VR content and interface with graphics hardware.

### **Dependency**
A software component or library that UEVR requires to function properly.

### **Deployment**
The process of distributing and installing UEVR to end users or target systems.

### **DLL (Dynamic Link Library)**
A Windows file format that contains code and data that can be used by multiple programs simultaneously.

### **Docker**
A platform for developing, shipping, and running applications in containers, providing consistency across different environments.

### **DRM (Digital Rights Management)**
Technology that controls access to digital content, which can sometimes interfere with UEVR's ability to modify games.

## E

### **Engine Adapter**
See **Adapter**.

### **Error Handling**
The process of detecting, responding to, and recovering from errors that occur during UEVR operation.

### **Event System**
A communication mechanism that allows different parts of UEVR to notify each other of important occurrences or state changes.

### **Extension**
A modular component that adds new functionality to UEVR, such as additional VR features or game engine support.

## F

### **Factory Pattern**
A design pattern used in UEVR to create objects without specifying their exact class, allowing for flexible object creation.

### **Frame Rate**
The number of frames displayed per second (FPS), critical for smooth VR experiences.

### **Framework**
The core architecture and components that provide the foundation for UEVR's functionality.

### **Frontend**
The user interface and user experience components of UEVR that users interact with directly.

## G

### **Git**
A distributed version control system used to track changes in UEVR's source code and collaborate with other developers.

### **GitHub**
A web-based platform for hosting Git repositories, where UEVR's source code and documentation are maintained.

### **GPU (Graphics Processing Unit)**
The hardware component responsible for rendering graphics, crucial for VR performance and quality.

### **Graphics API**
A programming interface that allows software to communicate with graphics hardware, such as DirectX, Vulkan, or OpenGL.

## H

### **Headset**
The VR display device worn on the head that provides stereoscopic 3D vision and head tracking.

### **Hook**
A mechanism that intercepts and modifies function calls or system events, allowing UEVR to inject VR functionality into games.

### **Hook Manager**
The UEVR component responsible for installing, managing, and removing hooks that enable VR functionality.

### **Hot Reload**
The ability to update UEVR functionality without restarting the application, useful for development and testing.

## I

### **IDE (Integrated Development Environment)**
Software that provides comprehensive tools for writing, testing, and debugging code, such as Visual Studio or VS Code.

### **Input Manager**
The UEVR component that handles VR controller input, translating physical movements into game actions.

### **IPD (Interpupillary Distance)**
The distance between the centers of the pupils of the eyes, used to configure VR headset optics for optimal viewing.

### **Injection**
See **Code Injection**.

### **Integration Testing**
Testing that verifies different components of UEVR work together correctly as a complete system.

## J

### **JSON (JavaScript Object Notation)**
A lightweight data format used for configuration files and data exchange in UEVR.

### **Jitter**
Unwanted movement or vibration in VR that can cause motion sickness or discomfort.

## L

### **Latency**
The time delay between user input and system response, critical for comfortable VR experiences.

### **Library**
A collection of pre-written code that provides common functionality, used by UEVR to avoid reinventing basic features.

### **Lua**
A lightweight scripting language used in UEVR for automation, configuration, and extending functionality.

### **Lua Engine**
The UEVR component that executes Lua scripts and provides scripting capabilities.

## M

### **Memory Management**
The process of allocating, using, and freeing computer memory efficiently, important for UEVR's performance and stability.

### **Mermaid**
A tool for creating diagrams and flowcharts in documentation, used throughout UEVR documentation.

### **Mod**
A modification or enhancement to UEVR that adds new features or changes existing behavior.

### **Mod Manager**
The UEVR component that loads, manages, and coordinates different mods and extensions.

### **Motion Sickness**
A condition that can occur in VR when there's a mismatch between visual and vestibular information.

### **MT Framework**
Capcom's proprietary game engine used in games like Monster Hunter and Resident Evil.

## N

### **Native Support**
Built-in functionality that doesn't require additional software or modifications to work.

### **Ninja**
A fast build system generator that can be used as an alternative to Make for building UEVR.

### **Node.js**
A JavaScript runtime environment used for build scripts and development tools in UEVR.

## O

### **Observer Pattern**
A design pattern used in UEVR where objects can subscribe to and receive notifications about events or state changes.

### **OpenGL**
A cross-platform graphics API that provides low-level access to graphics hardware.

### **OpenVR**
A VR software development kit and API that provides access to VR hardware and functionality.

### **OpenXR**
An open standard for VR and AR applications that provides cross-platform access to VR hardware and functionality.

### **Optimization**
The process of improving UEVR's performance, efficiency, and resource usage.

## P

### **Package Manager**
A tool that automates the process of installing, updating, and managing software dependencies.

### **Performance Monitor**
The UEVR component that tracks and reports on system performance, helping users optimize their VR experience.

### **Plugin**
A modular extension that adds new functionality to UEVR, similar to a mod but with a more formal interface.

### **Plugin Loader**
The UEVR component responsible for discovering, loading, and managing plugins.

### **Profiling**
The process of analyzing UEVR's performance to identify bottlenecks and optimization opportunities.

### **Proxy Pattern**
A design pattern that provides a surrogate or placeholder for another object, used in UEVR for lazy loading and access control.

## Q

### **Quality Assurance (QA)**
The process of ensuring UEVR meets quality standards through testing, validation, and verification.

### **Quick Start**
A simplified guide that helps users get UEVR running quickly with minimal setup and configuration.

## R

### **RAD (Rapid Application Development)**
A development methodology that emphasizes quick prototyping and iterative development, used in UEVR development.

### **Rendering Pipeline**
The sequence of steps that transform 3D scene data into 2D images for display, adapted for VR stereoscopic rendering.

### **Repository**
A storage location for UEVR's source code, documentation, and other project files, typically hosted on GitHub.

### **RE Engine**
Capcom's proprietary game engine used in Resident Evil 7, Resident Evil 8, and Devil May Cry 5.

### **REDengine**
CD Projekt Red's proprietary game engine used in The Witcher series and Cyberpunk 2077.

### **Rollback**
The process of reverting UEVR to a previous version or state, useful for recovering from failed updates or configurations.

### **Runtime**
The environment in which UEVR executes, including the operating system, libraries, and dependencies.

## S

### **SDK (Software Development Kit)**
A collection of tools, libraries, and documentation that developers use to create applications for a specific platform or technology.

### **Security Manager**
The UEVR component responsible for ensuring system security, access control, and integrity verification.

### **Shader**
A program that runs on the GPU to process graphics data, used by UEVR for VR rendering effects.

### **Shader Manager**
The UEVR component that manages and coordinates shader programs for VR rendering.

### **Singleton Pattern**
A design pattern that ensures a class has only one instance, used in UEVR for global managers and services.

### **Source Code**
The human-readable programming instructions that define UEVR's functionality and behavior.

### **SteamVR**
Valve's VR platform and runtime that provides access to VR hardware and functionality.

### **Stereoscopic Rendering**
The process of creating separate images for the left and right eyes to create a 3D effect in VR.

### **Strategy Pattern**
A design pattern that allows UEVR to select algorithms at runtime, useful for different deployment or rendering strategies.

## T

### **Target Platform**
The specific operating system, architecture, or environment where UEVR is intended to run.

### **Telemetry**
The collection and transmission of usage data and metrics, which UEVR does not collect for privacy reasons.

### **Testing Framework**
The collection of tools and methodologies used to verify UEVR's functionality, performance, and reliability.

### **Texture Manager**
The UEVR component responsible for loading, managing, and optimizing texture resources for VR rendering.

### **Threading**
The use of multiple execution threads to improve UEVR's performance and responsiveness.

### **Troubleshooting**
The process of diagnosing and resolving problems that users encounter with UEVR.

## U

### **UEVR**
Universal Engine VR Adapter, the main software that enables VR functionality in games not originally designed for VR.

### **Unit Testing**
Testing individual components or functions of UEVR in isolation to ensure they work correctly.

### **Unity Engine**
Unity Technologies' cross-platform game engine, with limited support in UEVR.

### **Update System**
The mechanism that allows UEVR to automatically or manually update to newer versions.

### **User Interface (UI)**
The visual elements and controls that users interact with to configure and control UEVR.

## V

### **Validation**
The process of verifying that UEVR is working correctly and meeting performance and quality requirements.

### **Version Control**
The management of changes to UEVR's source code over time, typically using Git.

### **Virtual Machine (VM)**
A software environment that emulates a computer system, useful for testing UEVR across different platforms.

### **VR (Virtual Reality)**
A technology that creates immersive, computer-generated environments that users can interact with.

### **VR Manager**
The UEVR component that coordinates VR functionality, including rendering, input, and tracking.

### **VR Runtime**
Software that provides access to VR hardware and manages VR applications, such as SteamVR or Oculus.

### **Vulkan**
A modern, low-overhead graphics API that provides high performance and cross-platform support.

## W

### **Webhook**
A mechanism that allows external systems to notify UEVR of events or trigger actions.

### **Windows Mixed Reality (WMR)**
Microsoft's VR platform that provides access to VR hardware and functionality on Windows.

### **Workflow**
A sequence of steps or processes that define how a particular task is accomplished in UEVR.

## X

### **XML (eXtensible Markup Language)**
A markup language used for configuration files and data exchange in some parts of UEVR.

## Y

### **YAML (YAML Ain't Markup Language)**
A human-readable data format used for configuration files in UEVR.

## Z

### **Zero-downtime Deployment**
A deployment strategy that allows UEVR to be updated without interrupting service or user experience.

---

## 📚 **Related Documentation**

- [Quick Start Guide](quick-start.md) - Get started with UEVR
- [API Reference](development/api-reference.md) - Technical implementation details
- [Architecture Overview](architecture/system-overview.md) - System design and structure
- [Troubleshooting Guide](troubleshooting.md) - Problem solving and solutions

---

*Last updated: January 15, 2024*  
*Glossary version: 2.0.0*
