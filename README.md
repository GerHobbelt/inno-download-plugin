# [IDP](https://mitrichsoftware.wordpress.com/inno-setup-tools/inno-download-plugin): Download plugin for Inno Setup #

[Inno Download Plugin](https://mitrichsoftware.wordpress.com/inno-setup-tools/inno-download-plugin) is a plugin for [Inno Setup](http://www.jrsoftware.org/isinfo.php), which allows you to download files as part of your installation.

### Features: ###

* Supports **Unicode** & **ANSI** Inno Setup versions
* **FTP**, **HTTP** and **HTTPS** protocols
* Configurable proxy settings
* Multiple languages
* Free and open source under [Zlib license](http://opensource.org/licenses/Zlib)

### Supported languages: ###

* Belarusian
* Brazilian Portuguese
* English
* Finnish
* French
* German
* Italian
* Polish
* Russian
* Simplified Chinese

### Basic example: ###

```
#include <idp.iss>

[Files]
Source: "{tmp}\file1.xyz"; DestDir: "{app}"; Flags: external; ExternalSize: 1048576
Source: "{tmp}\file2.xyz"; DestDir: "{app}"; Flags: external; ExternalSize: 1048576
Source: "{tmp}\file3.xyz"; DestDir: "{app}"; Flags: external; ExternalSize: 1048576

[Icons]
Name: "{group}\{cm:UninstallProgram,My Program}"; Filename: "{uninstallexe}"

[Code]
procedure InitializeWizard();
begin
    idpAddFileSize('http://127.0.0.1/file1.xyz', ExpandConstant('{tmp}\file1.xyz'), 1048576);
    idpAddFileSize('http://127.0.0.1/file2.xyz', ExpandConstant('{tmp}\file2.xyz'), 1048576);
    idpAddFileSize('http://127.0.0.1/file3.xyz', ExpandConstant('{tmp}\file3.xyz'), 1048576);

    idpDownloadAfter(wpReady);
end;
```

[More examples...](../../tree/master/examples)

### Download: ###

Latest version is 1.5.1 (17 Apr 2016) is available via [Google Drive](https://drive.google.com/folderview?id=0Bzw1xBVt0mokSXZrUEFIanV4azA&usp=sharing#list).

* [Download version 1.5.0 (Yandex Disk)](https://yadi.sk/d/B1GXlnwXdxggz)
* [Download other releases (Yandex Disk)](https://yadi.sk/d/y1tTqndxVf7Uh)
* [Download other releases (Google Drive)](https://drive.google.com/folderview?id=0Bzw1xBVt0mokSXZrUEFIanV4azA&usp=sharing#list)

### Links: ###

* [Homepage](https://mitrichsoftware.wordpress.com/inno-setup-tools/inno-download-plugin)
* [Discussions & support: Inno Download Plugin on Google Groups](https://groups.google.com/forum/#!forum/inno-download-plugin)

### Building from source ###

To build IDP from source, you need:

* Win32 C++ compiler (tested with VC++ 2005 & gcc 4.9.1)
* [Lua](http://lua.org) to build documentation
