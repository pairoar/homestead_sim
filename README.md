# Homestead 
  embedded playground with FreeRTOS

## Log Task
  - log_low("msg") : "msg"
  - log_mid("msg") : [timestamp] + "msg"
  - log_high("msg"): [timestamp] + [filename && line_number] + "msg"

## Build
  ```
  mkdir build
  cd build
  cmake ../
  make
  ```

## Run
  ```
  ./homestead_sim
  ```
