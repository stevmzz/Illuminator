#include <LiquidCrystal.h> // Biblioteca para manejar la pantalla LCD

class Componente {
  protected:
    int pin;       // Número de pin del componente
    bool estado;   // Estado del componente (encendido/apagado)

  public:
    // Constructor
    Componente(int numeroPin) {
      pin = numeroPin;
      estado = false;
    }

    // Métodos virtuales
    virtual void iniciar() {
      // Implementación base - puede ser sobrescrita por clases hijas
    }

    virtual void escribir(int valor) {
      // Implementación base - puede ser sobrescrita por clases hijas
    }

    virtual int leer() {
      // Implementación base - puede ser sobrescrita por clases hijas
      return 0;
    }

    // Métodos getter
    int obtenerPin() {
      return pin;
    }

    bool obtenerEstado() {
      return estado;
    }

    // Método setter
    void establecerEstado(bool nuevoEstado) {
      estado = nuevoEstado;
    }
};

// Clase LED
class LED : public Componente {
  public:
    // Constructor
    LED(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
      pinMode(pin, OUTPUT); // Configurar como salida --------------- (modificar)
    }

    void escribir(int valor) override {
      digitalWrite(pin, valor); // Encender/apagar el LED
      estado = valor;
    }
};

// Clase Sensor
class Sensor : public Componente {
  public:
    // Constructor
    Sensor(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
      // Implementación para inicializar un sensor
    }

    int leer() override {
      // Implementación para leer valores del sensor
      return 0;
    }
};

// Clase Pantalla
class Pantalla : public Componente {
  private:
    LiquidCrystal lcd; // Instancia del LCD
    int columnas;      // Número de columnas del LCD
    int filas;         // Número de filas del LCD

  public:
    // Constructor
    Pantalla(int rs, int enable, int d4, int d5, int d6, int d7, int cols, int rows) 
      : Componente(-1), lcd(rs, enable, d4, d5, d6, d7) {
      columnas = cols;
      filas = rows;
    }

    // Sobrescritura de métodos
    void iniciar() override {
      lcd.begin(columnas, filas); // Inicializar el LCD con las dimensiones
      lcd.clear();
      lcd.print("Sistema Iniciado"); // Mensaje inicial
      delay(2000);
      lcd.clear();
    }

    void mostrarModo(int modo) {
      lcd.clear(); // Limpiar la pantalla antes de mostrar nueva información

      // Mostrar el modo actual
      lcd.setCursor(0, 0); // Primera línea
      lcd.print("Modo Actual:");
      lcd.setCursor(0, 1); // Segunda línea
      switch (modo) {
        case 0:
          lcd.print("Noche");
          break;
        case 1:
          lcd.print("Lectura");
          break;
        case 2:
          lcd.print("Fiesta");
          break;
        case 3:
          lcd.print("Exterior");
          break;
        default:
          lcd.print("Desconocido");
          break;
      }
    }
};


// Clase Botón
class Boton : public Componente {
  private:
    int modoActual;               // Modo de iluminación actual
    unsigned long ultimoTiempo;   // Tiempo de la última interacción
    const unsigned long debounceDelay = 50; // Retardo de debounce

  public:
    // Constructor
    Boton(int numeroPin) : Componente(numeroPin) {
      modoActual = 0;
      ultimoTiempo = 0;
    }

    // Sobrescritura de métodos
    void iniciar() override {
      pinMode(pin, INPUT_PULLUP); // Configurar como entrada con resistencia pull-up
    }

    int leer() override {
      return !digitalRead(pin); // Retorna 1 cuando el botón está presionado
    }

    int cambiarModo() {
      int estadoBoton = leer();

      // Detectar cambio de estado con debounce
      if (estadoBoton == HIGH && (millis() - ultimoTiempo > debounceDelay)) {
        ultimoTiempo = millis();
        modoActual++; // Cambiar al siguiente modo
        if (modoActual > 3) modoActual = 0; // Reiniciar al primer modo
      }

      return modoActual;
    }
};

// Implementación en el programa principal *Falta configurar los pines acorde al ARDUINO*
LED luzSala(3);
LED luzPatio(4);
LED luzFiesta(5);
LED luzExterior(6);
Boton boton(2);

void activarModo(int modo) {
  // Apagar todas las luces
  luzSala.escribir(LOW);
  luzPatio.escribir(LOW);
  luzFiesta.escribir(LOW);
  luzExterior.escribir(LOW);

  // Activar luces según el modo
  switch (modo) {
    case 0: // Modo Noche
      // Todas apagadas
      break;
    case 1: // Modo Lectura
      luzSala.escribir(HIGH);
      break;
    case 2: // Modo Fiesta
      for (int i = 0; i < 10; i++) {
        luzSala.escribir(HIGH);
        luzPatio.escribir(HIGH);
        luzFiesta.escribir(HIGH);
        luzExterior.escribir(HIGH);
        delay(200);
        luzSala.escribir(LOW);
        luzPatio.escribir(LOW);
        luzFiesta.escribir(LOW);
        luzExterior.escribir(LOW);
        delay(200);
      }
      break;
    case 3: // Modo Exterior
      luzPatio.escribir(HIGH);
      break;
  }
}

void setup() {
  // Inicializar componentes
  boton.iniciar();
  luzSala.iniciar();
  luzPatio.iniciar();
  luzFiesta.iniciar();
  luzExterior.iniciar();
}

void loop() {
  // Cambiar modo según el botón
  int modo = boton.cambiarModo();

  // Activar el modo correspondiente
  activarModo(modo);

  delay(100); // Reducir frecuencia del bucle principal
}