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
    int umbralLuz;  // Umbral para la detección de luz
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
    // Método para establecer el umbral de luz
    void establecerUmbralLuz(int nuevoUmbral) {
      umbralLuz = nuevoUmbral;
    }
    // Método para obtener el umbral de luz
    int obtenerUmbralLuz() {
      return umbralLuz;
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
  private:
    unsigned long ultimoTiempoBoton;  // Para el control de rebote
    const long tiempoDebounce = 200;  // Tiempo de debounce en milisegundos
    int estadoAnterior;               // Para detectar cambios

  public:
    // Constructor
    Boton(int numeroPin) : Componente(numeroPin) {
      ultimoTiempoBoton = 0;
      estadoAnterior = HIGH;
    }
    // Sobrescritura de métodos
    void iniciar() override {
      pinMode(pin, INPUT_PULLUP);
      estadoAnterior = digitalRead(pin);
    }
    int leer() override {
      int estadoActual = digitalRead(pin);
      if ((millis() - ultimoTiempoBoton) > tiempoDebounce) {
        if (estadoActual == LOW && estadoAnterior == HIGH) {
          ultimoTiempoBoton = millis();
          estado = !estado;  // Cambiar estado del botón
        }
      }
      estadoAnterior = estadoActual;
      return estado;
    }
};

// Variables globales para el control de iluminación automática
bool modoAutomatico = true;  // Estado del modo automático

// Configuración inicial del sistema
void setup() {
    Serial.begin(9600);  // Iniciar comunicación serial para monitoreo

    // Crear e inicializar componentes
    Sensor sensorLuz(A0);
    LED ledsHabitaciones[] = {
        LED(2), LED(3), LED(4), LED(5), 
        LED(6), LED(7), LED(8)
    };
    Boton botonControl(0);

    // Inicializar componentes
    sensorLuz.iniciar();
    botonControl.iniciar();
    for(int i = 0; i < 7; i++) {
        ledsHabitaciones[i].iniciar();
    }
}

// Bucle principal del sistema
void loop() {
    // Crear instancias de los componentes necesarios
    Sensor sensorLuz(A0);
    Boton botonControl(0);
    LED ledsHabitaciones[] = {
        LED(2), LED(3), LED(4), LED(5), 
        LED(6), LED(7), LED(8)
    };

    // Verificar cambio en el modo automático
    if(botonControl.leer()) {
        modoAutomatico = !modoAutomatico;
        Serial.print("Modo Automático: ");
        Serial.println(modoAutomatico ? "Activado" : "Desactivado");
    }

    // Procesar el control automático de iluminación
    if (modoAutomatico) {
        int valorLuz = sensorLuz.leer();
        // Controlar todos los LEDs según el nivel de luz
        for(int i = 0; i < 7; i++) {
            ledsHabitaciones[i].escribir(
                valorLuz >= sensorLuz.obtenerUmbralLuz() ? HIGH : LOW
            );
        }
        
        // Imprimir información de monitoreo
        Serial.print("Sensor: ");
        Serial.print(valorLuz);
        Serial.print(" - LEDs: ");
        Serial.println(valorLuz >= sensorLuz.obtenerUmbralLuz() ? 
                      "Encendidos" : "Apagados");
    }

    delay(100);  // Pequeña pausa para estabilidad
}