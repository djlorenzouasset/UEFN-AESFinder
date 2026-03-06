# UEFN-AESFinder

Dll that allows you to dump the main AES key of Fortnite through the Unreal Editor (UEFN). I wanted to port [AESDumpster](https://github.com/GHFear/AESDumpster) (console application) to a UEFN specific DLL to inject into the game. Special thanks to [@notkrae](https://x.com/notkrae) for helping & testing all* the UEFN versions.

> [!WARNING]
> Do not inject this DLL in the main game exe. Instead, use UEFN with [Carbon](https://discord.gg/vj3AW6NC)

## Installation

1. Clone the source code with [git](https://git-scm.com/downloads): `git clone https://github.com/djlorenzouasset/UEFN-AESFinder`
2. Build the project using your preferred IDE.

## How to use

1. Run Carbon private server (get it from [here](https://discord.gg/vj3AW6NC)).
2. Use your preferred injector to inject the DLL in the UEFN process (UnrealEditorFortnite-Win64-Shipping.exe).
3. Once the DLL gets injected a file (`aes_list.txt`) will be saved in the directory `C:/UEFN-AESFinder/` containing all found keys.

### Change save path

To change the location where the AES keys are saved, open the file `main.cpp` (`../UEFN-AESFinder/Private/main.cpp`) and edit the `OutputPath` var with the preferred location.

## Contributing 
Contributions are welcome, feel free to create pull requests.
