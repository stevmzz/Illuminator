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
        digitalWrite(pin, LOW); // Inicializa el LED apagado
    }

    void escribir(int valor) override {
        estado = valor;
        digitalWrite(pin, valor ? HIGH : LOW);
    }
};

// Clase Sensor
class Sensor : public Componente {
  public:
    // Constructor
    Sensor(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
        pinMode(pin, INPUT);
    }

    int leer() override {
        return analogRead(pin); // Leer el valor analógico
    }

};

// Clase Pantalla
class Pantalla : public Componente {
  public:
    // Constructor
    Pantalla(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
      // Implementación para inicializar una pantalla
    }

    void escribir(int valor) override {
      // Implementación para escribir valores en la pantalla
    }
};

// Clase Botón
class Boton : public Componente {
  public:
    // Constructor
    Boton(int numeroPin) : Componente(numeroPin) {}

    // Sobrescritura de métodos
    void iniciar() override {
        pinMode(pin, INPUT_PULLUP);
    }

    int leer() override {
        return digitalRead(pin);
    }
};

const int cantidad = 6;
LED leds[cantidad] = {7, 6, 5, 4, 3, 2};
Sensor sensores[cantidad] = {A0, A1, A2, A3, A4, A5};
Boton boton(8);

bool modoAutomatico = true;
unsigned long ultimoTiempoBoton = 0;
const long tiempoDebounce = 200;

void setup() {
    for (int i = 0; i < cantidad; i++) {
        leds[i].iniciar();
        sensores[i].iniciar();
    }
    boton.iniciar();
    Serial.begin(9600);
}

void loop() {
    // Lógica del botón
    if ((millis() - ultimoTiempoBoton) > tiempoDebounce) {
        if (boton.leer() == LOW && !modoAutomatico) {
            modoAutomatico = !modoAutomatico;
            Serial.println(modoAutomatico ? "Modo automático activado" : "Modo automático desactivado");
            ultimoTiempoBoton = millis();
        }
    }

    // Control automático de LEDs
    if (modoAutomatico) {
        for (int i = 0; i < cantidad; i++) {
            int luz = sensores[i].leer();
            leds[i].escribir(luz < 500); // Encender si luz ambiental es baja
            Serial.print("Sensor ");
            Serial.print(i);
            Serial.print(": ");
            Serial.print(luz);
            Serial.print(" - LED ");
            Serial.print(i);
            Serial.println(luz < 500 ? "Encendido" : "Apagado");
        }
    }

    delay(100); // Estabilización
}
