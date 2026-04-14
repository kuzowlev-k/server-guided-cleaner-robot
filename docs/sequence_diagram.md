@startuml
Server -->> Camera: проверить соединение
Server -->> Robot : проверить соединение
Server <<- Camera: видео
Server -> Server: постороение маршрута
Server -> Robot : вперед 
Server -> Robot : назад
Server -> Robot : вправо 
Server -> Robot : влево
@enduml