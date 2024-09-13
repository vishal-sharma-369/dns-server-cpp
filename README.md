# Creating our own DNS from scratch in CPP

A Secure and Scalable Microservice Based DNS Server
with DoH and DNSSEC Support

## Objectives:

- Implementation of the DNS protocol using UDP sockets, packet parsers and writers
- Implementation of parsers and resolvers for various record types
- Implementation of recursive lookups
- Implementation of DOH and DNSSec Services
- Implementation of Cache Poisoning Prevention techniques
- Implementation of the Microservice architecture for effortless scalability
- Implementation of Load Balancing to balance the load across multiple servers

## Pre-requisites:

- [make](https://www.gnu.org/software/make/)

- [CMake](https://cmake.org/)

- [vcpkg](https://vcpkg.io/en/)

## Steps to Install Pre-requisites:

- ### Installing make

Here’s how to install `make` on various operating systems:

#### Ubuntu/Debian-based Systems:

```bash
sudo apt update
sudo apt install make
```

#### Fedora/RHEL-based Systems:

```bash
sudo dnf install make
```

For RHEL/CentOS:

```bash
sudo yum install make
```

#### Arch Linux:

```bash
sudo pacman -S make
```

#### macOS:

`make` is included with Xcode Command Line Tools. Install it using:

```bash
xcode-select --install
```

#### Windows:

- ##### **Using Chocolatey**:

  ```bash
  choco install make
  ```

- ##### **Using MSYS2**:

  - First, install MSYS2 from [MSYS2 website](https://www.msys2.org/).
  - Open the MSYS2 shell and update the package database:
    ```bash
    pacman -Syu
    ```
  - Install `make`:
    ```bash
    pacman -S make
    ```

- ##### **Using WSL (Windows Subsystem for Linux)**:
  If you have WSL installed, you can follow the Linux installation instructions for `make` within the WSL environment.

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

- ### Installing CMake

Here's how to install CMake on various operating systems:

#### Ubuntu/Debian-based Systems:

- **Using APT (default repository)**:

  ```bash
  sudo apt update
  sudo apt install cmake
  ```

- **Using APT for the latest version**:
  - You might need to add a PPA (Personal Package Archive) for the latest version:
    ```bash
    sudo apt update
    sudo apt install software-properties-common
    sudo add-apt-repository ppa:kitware/ppa
    sudo apt update
    sudo apt install cmake
    ```

#### Fedora/RHEL-based Systems:

- **Using DNF**:

  ```bash
  sudo dnf install cmake
  ```

- **For RHEL/CentOS (EPEL repository may be needed)**:
  ```bash
  sudo yum install cmake
  ```

#### Arch Linux:

- **Using Pacman**:
  ```bash
  sudo pacman -S cmake
  ```

#### macOS

- **Using Homebrew**:

  - If you don’t have Homebrew, install it first:
    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```
  - Then install CMake:
    ```bash
    brew install cmake
    ```

- **Using MacPorts**:
  ```bash
  sudo port install cmake
  ```

#### Windows

- **Using Chocolatey**:

  ```bash
  choco install cmake
  ```

- **Using the Official Installer**:

  - Download the latest installer from the [CMake website](https://cmake.org/download/).
  - Run the installer and follow the setup instructions.

- **Using MSYS2**:
  - Install MSYS2 from the [MSYS2 website](https://www.msys2.org/).
  - Open the MSYS2 shell and install CMake:
    ```bash
    pacman -S cmake
    ```

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

- ### Installing vcpkg

Here’s a comprehensive guide for installing **vcpkg** on various operating systems, including setting up the `VCPKG_ROOT` environment variable:

#### Ubuntu/Debian-based Systems:

- **Install dependencies**:

  ```bash
  sudo apt update
  sudo apt install build-essential curl zip unzip
  ```

- **Clone the vcpkg repository and bootstrap**:

  ```bash
  git clone https://github.com/microsoft/vcpkg.git
  cd vcpkg
  ./bootstrap-vcpkg.sh
  ```

- **Add `vcpkg` to your PATH** (optional but recommended):

  ```bash
  export PATH="$PATH:/path/to/vcpkg"
  ```

- **Set the `VCPKG_ROOT` environment variable**:
  - Open the `.bashrc` or `.zshrc` file in a text editor:
    ```bash
    nano ~/.bashrc
    # or
    nano ~/.zshrc
    ```
  - Add the following line at the end of the file:
    ```bash
    export VCPKG_ROOT=/path/to/vcpkg
    ```
  - Apply the changes:
    ```bash
    source ~/.bashrc
    # or
    source ~/.zshrc
    ```

#### Fedora/RHEL-based Systems:

- **Install dependencies**:

  ```bash
  sudo dnf install gcc-c++ curl zip unzip git
  ```

- **Clone the vcpkg repository and bootstrap**:

  ```bash
  git clone https://github.com/microsoft/vcpkg.git
  cd vcpkg
  ./bootstrap-vcpkg.sh
  ```

- **Add `vcpkg` to your PATH** (optional but recommended):

  ```bash
  export PATH="$PATH:/path/to/vcpkg"
  ```

- **Set the `VCPKG_ROOT` environment variable**:
  - Open the `.bashrc` or `.zshrc` file in a text editor:
    ```bash
    nano ~/.bashrc
    # or
    nano ~/.zshrc
    ```
  - Add the following line at the end of the file:
    ```bash
    export VCPKG_ROOT=/path/to/vcpkg
    ```
  - Apply the changes:
    ```bash
    source ~/.bashrc
    # or
    source ~/.zshrc
    ```

#### Arch Linux:

- **Install dependencies**:

  ```bash
  sudo pacman -S base-devel git curl unzip
  ```

- **Clone the vcpkg repository and bootstrap**:

  ```bash
  git clone https://github.com/microsoft/vcpkg.git
  cd vcpkg
  ./bootstrap-vcpkg.sh
  ```

- **Add `vcpkg` to your PATH** (optional but recommended):

  ```bash
  export PATH="$PATH:/path/to/vcpkg"
  ```

- **Set the `VCPKG_ROOT` environment variable**:
  - Open the `.bashrc` or `.zshrc` file in a text editor:
    ```bash
    nano ~/.bashrc
    # or
    nano ~/.zshrc
    ```
  - Add the following line at the end of the file:
    ```bash
    export VCPKG_ROOT=/path/to/vcpkg
    ```
  - Apply the changes:
    ```bash
    source ~/.bashrc
    # or
    source ~/.zshrc
    ```

#### macOS:

- **Install dependencies** (Homebrew can be used to install curl and unzip):

  ```bash
  brew install git curl unzip
  ```

- **Clone the vcpkg repository and bootstrap**:

  ```bash
  git clone https://github.com/microsoft/vcpkg.git
  cd vcpkg
  ./bootstrap-vcpkg.sh
  ```

- **Add `vcpkg` to your PATH** (optional but recommended):

  ```bash
  export PATH="$PATH:/path/to/vcpkg"
  ```

- **Set the `VCPKG_ROOT` environment variable**:
  - Open the `.bash_profile`, `.bashrc`, or `.zshrc` file in a text editor (depending on your shell):
    ```bash
    nano ~/.bash_profile
    # or
    nano ~/.zshrc
    ```
  - Add the following line at the end of the file:
    ```bash
    export VCPKG_ROOT=/path/to/vcpkg
    ```
  - Apply the changes:
    ```bash
    source ~/.bash_profile
    # or
    source ~/.zshrc
    ```

#### Windows:

- **Install Git** (if not already installed): [Download Git for Windows](https://git-scm.com/download/win).

- **Install vcpkg**:

  - Open Command Prompt or PowerShell.
  - Clone the vcpkg repository:
    ```bash
    git clone https://github.com/microsoft/vcpkg.git
    ```
  - Navigate to the `vcpkg` directory and bootstrap:
    ```bash
    cd vcpkg
    .\bootstrap-vcpkg.bat
    ```

- **Add `vcpkg` to your PATH** (optional but recommended):

  - You can set the PATH variable in the system environment variables, or use the `vcpkg integrate install` command to integrate vcpkg with Visual Studio.

- **Set the `VCPKG_ROOT` environment variable**:

  - Open the System Properties:
    - Right-click on `This PC` or `Computer` on the desktop or in File Explorer.
    - Select `Properties`.
    - Click on `Advanced system settings`.
    - Click on `Environment Variables`.
  - Add the `VCPKG_ROOT` environment variable:
    - In the "System variables" section, click `New`.
    - Set the **Variable name** to `VCPKG_ROOT`.
    - Set the **Variable value** to the path of your `vcpkg` installation (e.g., `C:\path\to\vcpkg`).
    - Click `OK` to save.

- **Apply the changes**:
  - Click `OK` to close the Environment Variables window.
  - Click `OK` to close the System Properties window.

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

---

## Project Setup on Local Development Environment

- Move to the root directory of the project and execute the command

```bash
  ./your_program.sh
```

- For testing purposes: Execute the command

```bash
  ./test.sh
```
