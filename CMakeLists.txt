cmake_minimum_required(VERSION 3.15) # Проверка версии CMake.
									 # Если версия установленной программы
									 # старее указанной, произойдёт аварийный выход.

set(PROJECT_NAME HeadToFace)				 # задать константу с именем проекта		 
project("${PROJECT_NAME}")			 # имя проекта	


set(CMAKE_CXX_STANDARD 17)			 # устанавливаем 17 стандарт языка
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include_directories("opencv/include/")
link_directories("opencv/lib")		 # указываем, где искать файлы для opencv

include_directories("asio/include/")
link_directories("asio/lib")		 # указываем, где искать файлы для asio

add_executable("${PROJECT_NAME}" main.cpp
				wheel-driver/io/SerialConnector.cpp
				wheel-driver/protocol/ModbusMaster.cpp
				wheel-driver/io/SerialConnector.hpp
				wheel-driver/protocol/ModbusMaster.hpp
				)	# добавить исполняемый файл
        
target_link_libraries("${PROJECT_NAME}" optimized opencv_world470.lib)
target_link_libraries("${PROJECT_NAME}" debug opencv_world470d.lib)
						  
						  
file(COPY "res" DESTINATION "/")
file(COPY "opencv" DESTINATION "/")	
file(COPY "asio" DESTINATION "/")
file(COPY "wheel-driver" DESTINATION "/")
						 