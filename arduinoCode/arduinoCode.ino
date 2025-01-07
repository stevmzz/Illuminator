// Variables globales
bool modoAutomatico = true;  // Estado del modo automático

class Componente {
  protected:
    int pin; // Número de pin del componente
    bool estado; // Estado del componente

  public:
    // Constructor
    Componente(int numeroPin) {
      pin = numeroPin; // Asigna el número de pin recibido
      estado = false; // Inicialmente, el componente está apagado
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
    
    void iniciar() override {
      pinMode(pin, OUTPUT); // Configura el pin como salida
      digitalWrite(pin, LOW); // Inicialmente apaga el LED
      estado = false; // Actualiza el estado interno
    }
    
    void escribir(int valor) override {
      digitalWrite(pin, valor); // Escribe el valor en el pin sea HIGH o LOW
      estado = (valor == HIGH); // Actualiza el estado
    }
};

// Clase Sensor
class Sensor : public Componente {
  private:
    int umbralLuz; // Umbral para la detección de luz

  public:
    // Constructor
    Sensor(int numeroPin) : Componente(numeroPin) {
      umbralLuz = 5; // Valor predeterminado del umbral ajustado para condiciones de luz
    }

    void iniciar() override {
      pinMode(pin, INPUT); // Configura el pin como entrada
    }

    int leer() override {
      return analogRead(pin); // Lee el valor analógico del sensor
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
    unsigned long ultimoTiempoBoton; // Para el control de rebote
    const long tiempoDebounce = 2000; // Tiempo de debounce en ms
    int estadoAnterior; // Para detectar cambios

  public:
    // Constructor
    Boton(int numeroPin) : Componente(numeroPin) {
      ultimoTiempoBoton = 0; // Inicia el tiempo en cereo
      estadoAnterior = HIGH; // Inicializa el estado anterior
    }

    // Sobrescritura de métodos
    void iniciar() override {
      pinMode(pin, INPUT_PULLUP); // Configura el pin como entrada con resistencia pull-up
      estadoAnterior = digitalRead(pin); // Lee el estado inicial
    }

    int leer() override {
      int estadoActual = digitalRead(pin); // Lee el estado actual del botón

      if ((millis() - ultimoTiempoBoton) > tiempoDebounce) { // Verifica si ha pasado suficiente tiempo
        if (estadoActual == LOW && estadoAnterior == HIGH) { // Si el botón se presiona
          ultimoTiempoBoton = millis(); // Actualiza el tiempo de la última pulsación
          estado = !estado; // Cambiar estado del botón
        }
      }
      estadoAnterior = estadoActual; // Actualiza el estado anterior
      return estado; // Retorna el estado actual
    }
};

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
    sensorLuz.establecerUmbralLuz(5); // Establecer umbral ajustado para las condiciones actuales
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
        int valorLuz = sensorLuz.leer(); // Lee el valor del sensor
        // Controlar todos los LEDs según el nivel de luz
        for(int i = 0; i < 7; i++) {
            ledsHabitaciones[i].escribir(
                valorLuz > sensorLuz.obtenerUmbralLuz() ? HIGH : LOW // Enciende los LEDs cuando el valor es mayor que el umbral
            );
        }
        
        // Imprimir información de monitoreo
        Serial.print("Sensor: ");
        Serial.print(valorLuz);
        Serial.print(" - LEDs: ");
        Serial.println(valorLuz > sensorLuz.obtenerUmbralLuz() ? 
                      "Encendidos" : "Apagados");
    }

    delay(100);  // Pequeña pausa para estabilidad
}