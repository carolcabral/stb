# Lumen Corset

Esse projeto foi desenvolvido inicialmente para o festival de música eletrônica Só Track Boa em São Paulo.


- Comportamento padrão: 
  - iterar em todas as animações genéricas durante 1 minuto em cada um
  - a cada 30 min, mandar tomar água (controlado por timer)

- Controle por ble:
  - escolhe animações especiais, como I LOVE LUCAS
  - escolhe um das genéricas
  - se mantem na animação escolhida por 5 minutos e depois volta para o comportamento padrão
  

This example demonstrates how to blink a LED by using the GPIO driver or using the [led_strip](https://components.espressif.com/component/espressif/led_strip) library if the LED is addressable e.g. [WS2812](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf). The `led_strip` library is installed via [component manager](main/idf_component.yml).



