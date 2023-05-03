# Proyecto-Memoria

_Este proyecto se basa en la creación de un programa que emule la implementación de un sistema que permita medir el rendimiento de la aplicación de las tres técnicas de Administración de Memoria Sin Intercambio, esto para el curso de Sistemas Operativos de la carrera de Ingeniería en Sistemas (3 Nivel). El Programa incluye la Administración de Particiones Fijas de Varios Tamaños y de Particiones Variables, las cuáles son: Mapas de Bits, Listas Ligadas y Sistemas Socios_

---

## Comenzando 🚀

_Estas instrucciones te permitirán obtener un entorno de desarrollo Ubuntu completo en tu sistema Windows sin tener que hacer una instalación del mismo._

### Pre-requisitos 📋

* **Windows Subsystem for Linux (WSL):** _Tener activa la Virtualización en las Configuraciones del BIOS_
* **Visual Studio Code:** _Cumplir con los requisitos mínimos mencionados en [su página](https://code.visualstudio.com/Docs/supporting/requirements)_

### Instalación 🔧

**Windows Subsystem for Linux (WSL):** _El primer paso es instalar el entorno en el cual vamos a estar ejecutando nuestro código. Para esto, lo primero que debemos hacer es lo siguiente:_

1. Abre el menú de incio, busca ```cmd```, y ejecútalo como Administrador (```Click Derecho >> Ejecutar como Administrador```)

2. Una vez abierta la ventana asegúrate de que esta diga: ```C:\Windows\System32>```

3. Ahora instalamos WSL. Instalaremos de forma predeterminada la distro Ubuntu. Escribimos: ```wsl --install```. _Esperamos a que finalice la instalación. Una vez finalizada nos solicitará un usuario y contraseña, los cuales debemos recordar para más adelante._

4. Una vez instalado (y ejecutándose) Ubuntu, escribimos ```sudo su``` y escribimos nuestra contraseña. Una vez hecho esto escribimos los comandos ```apt-get update``` y ```apt-get upgrade```, para buscar e instalar actualizaciones respectivamente.

5. Por último, debemos escribir el comando _```sudo chown -R nombre_de_usuario /home/nombre_de_usuario/```_, cambiando _```nombre_de_usuario```_ por el nombre que hayan escogido al iniciar por primera vez. Con esto ya tendríamos Ubuntu actualizado y listo para trabajar.

**Compilador:** _Para instalar el compilador de C (GCC), sencillamente debemos de introducir el comando ```sudo apt install gcc```, y ya con esto estaría instalado._

**Visual Studio Code:**

1. Lo primero que debemos hacer es instalar el programa en Windows, para esto nos dirigimos a **_[su página](https://code.visualstudio.com/)_** y descargamos el programa y lo instalamos siguiendo sus indicaciones. (_Cuando se le solicite Seleccionar tareas adicionales durante la instalación, asegúrese de marcar la opción **Add to Path** para que pueda abrir fácilmente una carpeta en WSL._)

2. Una vez instalado, debemos dirigirnos al apartado de extensiones y buscar la extensión WSL, o puedes instalarla directamente desde **_[este enlace](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-wsl)_**.

3. Ya instalada la extensión, cerramos Visual Studio Code y abrimos Ubuntu (_aparece en el menú de Windows como una aplicación más_). Dentro de Ubuntu escribimos el comando _```code .```_ (_incluyendo el punto al final_) y esperamos a que descargue el servidor y arranque de nuevo Visual Studio Code.

Y listo, ya con esto tendríamos nuestro entorno de desarrollo listo para trabajar. Podemos presionar las teclas ```Win + Ñ``` para abrir una ventana del terminal dentro de Visual, o directamente ir a ```Terminal >> Nuevo Terminal``` opción que aparece en la barra de herramientas de arriba.

* **PD:** _Para dejar de usar Visual Studio Code en Ubuntu y pasar a usarlo con Windows, solamente debemos dar Click en el botón que aparece en la esquina inferior izquierda del Visual (que dice ```WSL: Ubuntu```) y seleccionar la opción que dice ```Cerrar conexión remota```. De igual forma, para volver entrar podemos darle al mismo botón y dar click donde dice ```Conectar a WSL```, o en Ubuntu abrir la carpeta en la que queramos trabajar (con ```cd```) y escribimos de nuevo el comando ```code .```_

---

## Ejecutando las pruebas ⚙️

_Para probar que todo haya quedado correctamente instalado, podemos realizar las siguientes pruebas:_

1. **Revisar que el Compilador (GCC) se haya instalado correctamente:** _En Ubuntu, escribir:_

```
gcc --version
```

_Debería aparecer un mensaje como el siguiente:_

    gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0
    Copyright (C) 2021 Free Software Foundation, Inc.
    This is free software; see the source for copying conditions.  There is NO
    warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

2. **Probar que se compile correctamente:** _Para empezar pueden crear un archivo ```main.c```, que contenga el siguiente código de prueba:_

```
#include <stdio.h>

int main () {
    printf("Hello, world!!\n");
    return 0;
}
```

_Una vez creado el archivo, abrimos la terminal de Visual Studio (```Win + Ñ```) y escribimos lo siguiente en orden (uno por uno):_

```
gcc -o main main.c
./main
```

_El archivo debería compilar y mostrarles el mensaje ```Hello, world!!``` en la terminal_

---

## Construido con 🛠️

_Herramientas que se utilizaron para crear el juego:_

* [Visual Studio Code](https://code.visualstudio.com/) - El Editor Utilizado
* [Windows Subsystem for Linux](https://learn.microsoft.com/en-us/windows/wsl/install) - Para la Ejecución del Código

## Autores ✒️

_Los integrantes que conformamos el grupo somos los siguientes:_

* **Isaac Herrera**  - *[Moshe9647](https://github.com/MOSHE9647)*
* **Melanie Oviedo** - *[Melanie_OM](#fulanito-de-tal)*
* **Jason Madrigal** - *[Jason_MO](#fulanito-de-tal)*
* **Gonzalo Dormos** - *[Gonzalo_D](#fulanito-de-tal)*

---
Escrito por [Moshe9647](https://github.com/MOSHE9647) 🔥