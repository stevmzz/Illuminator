#include <Adafruit_LiquidCrystal.h> // Biblioteca Adafruit para manejar la pantalla LCD

// Clase base para todos los componentes
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
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
      estado = false;
    }

    void escribir(int valor) override {
      digitalWrite(pin, valor);
      estado = (valor == HIGH);
    }
};

// Clase Sensor
class Sensor : public Componente {
  private:
    int umbralLuz;

  public:
    // Constructor
    Sensor(int numeroPin) : Componente(numeroPin) {
      umbralLuz = 500;  // Valor predeterminado del umbral
    }

    // Sobrescritura de métodos
    void iniciar() override {
      pinMode(pin, INPUT);
    }

    int leer() override {
      return analogRead(pin);
    }

    // Métodos para el umbral de luz
    void establecerUmbralLuz(int nuevoUmbral) {
      umbralLuz = nuevoUmbral;
    }

    int obtenerUmbralLuz() {
      return umbralLuz;
    }
};

// Clase Pantalla
class Pantalla : public Componente {
  private:
    Adafruit_LiquidCrystal lcd; // Objeto LCD de la biblioteca Adafruit
    int columnas;
    int filas;

  public:
    // Constructor
    Pantalla(int rs, int enable, int d4, int d5, int d6, int d7, int cols, int rows) 
      : Componente(-1), lcd(rs, enable, d4, d5, d6, d7) {
      columnas = cols;
      filas = rows;
    }

    // Sobrescritura de métodos
    void iniciar() override {
      lcd.begin(columnas, filas);
      lcd.clear();
      lcd.print("Sistema Iniciado");
      delay(2000);
      lcd.clear();
    }

    void mostrarModoYValor(int modo, int valorLuz) {
      lcd.clear(); // Limpiar la pantalla antes de mostrar nueva información
      lcd.setCursor(0, 0);
      lcd.print("Modo: ");
      switch (modo) {
        case 0: lcd.print("Noche"); break;
        case 1: lcd.print("Lectura"); break;
        case 2: lcd.print("Fiesta"); break;
        case 3: lcd.print("Exterior"); break;
        default: lcd.print("Desconocido"); break;
      }

      lcd.setCursor(0, 1); // Segunda línea
      lcd.print("Sensor: ");
      lcd.print(valorLuz); // Mostrar el valor del sensor
    }
};

// Clase Botón
class Boton : public Componente {
  private:
    int modoActual;
    unsigned long ultimoTiempo;
    const unsigned long debounceDelay = 50;

  public:
    // Constructor
    Boton(int numeroPin) : Componente(numeroPin) {
      modoActual = 0;
      ultimoTiempo = 0;
    }

    // Sobrescritura de métodos
    void iniciar() override {
      pinMode(pin, INPUT_PULLUP);
    }

    int leer() override {
      return !digitalRead(pin);
    }

    int cambiarModo() {
      int estadoBoton = leer();
      if (estadoBoton == HIGH && (millis() - ultimoTiempo > debounceDelay)) {
        ultimoTiempo = millis();
        modoActual++;
        if (modoActual > 3) modoActual = 0;
      }
      return modoActual;
    }
};

// Variables globales
LED luzSala(3);
LED luzPatio(4);
LED luzFiesta(5);
LED luzExterior(6);
Boton boton(2);
Sensor sensorLuz(A0);
Pantalla pantalla(12, 11, 5, 4, 3, 2, 16, 2);
bool modoAutomatico = true;

void activarModo(int modo) {
  luzSala.escribir(LOW);
  luzPatio.escribir(LOW);
  luzFiesta.escribir(LOW);
  luzExterior.escribir(LOW);

  pantalla.mostrarModoYValor(modo, sensorLuz.leer()); // Mostrar modo y valor del sensor

  switch (modo) {
    case 0: break;
    case 1: luzSala.escribir(HIGH); break;
    case 2:
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
    case 3: luzPatio.escribir(HIGH); break;
  }
}

void setup() {
  Serial.begin(9600);
  boton.iniciar();
  luzSala.iniciar();
  luzPatio.iniciar();
  luzFiesta.iniciar();
  luzExterior.iniciar();
  sensorLuz.iniciar();
  pantalla.iniciar();
}

void loop() {
  int modo = boton.cambiarModo();

  if (modoAutomatico) {
    int valorLuz = sensorLuz.leer();
    for (int i = 0; i < 7; i++) {
      luzSala.escribir(valorLuz >= sensorLuz.obtenerUmbralLuz() ? HIGH : LOW);
    }
    Serial.print("Sensor: ");
    Serial.print(valorLuz);
    Serial.print(" - LEDs: ");
    Serial.println(valorLuz >= sensorLuz.obtenerUmbralLuz() ? "Encendidos" : "Apagados");
    pantalla.mostrarModoYValor(modo, valorLuz); // Mostrar modo y valor en la pantalla
  } else {
    activarModo(modo);
  }

  delay(100);
}
