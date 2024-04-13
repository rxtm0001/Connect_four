import cv2 as cv
import numpy as np
import random
import sys
import math
import time
import serial.tools.list_ports


#------------------------------------------------------#
#Constantes Connect Four
#------------------------------------------------------#
BLUE = (0,0,255)
BLACK = (0,0,0)
RED = (255,0,0)
YELLOW = (255,255,0)

ROW_COUNT = 6
COLUMN_COUNT = 7

PLAYER = 0
AI = 1

EMPTY = 0
PLAYER_PIECE = 1
AI_PIECE = 2

WINDOW_LENGTH = 4
#------------------------------------------------------#

#----------------------------------------------------------------------------------------------------------------------------------------------------------------#
#Constantes openCV
#----------------------------------------------------------------------------------------------------------------------------------------------------------------#
mask_x_ini = 50
mask_y_ini = 50

increment_x_ini = 84
increment_y_ini = 75

increment_x2 = 95
increment_y2 = 80

lowerLimitY = np.array([15,0,0])
upperLimitY = np.array([35,255,255])

lowerLimitR1 = np.array([150,0,0])
upperLimitR1 = np.array([180,255,255])
lowerLimitR2 = np.array([0,0,0])
upperLimitR2 = np.array([10,255,255])

a = increment_x_ini*2
b = increment_y_ini*2
horizontal_lines = [50, 125, 200, 275, 350, 425]
vertical_lines = [50, 134, 218, 302, 386, 470, 554]
#---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------#

#------------------------------------------------------#
#Timer


def current_milli_time():
    return round(time.time() * 1000)

robot_time = 0
current_time = 0
#------------------------------------------------------#


def getTurn():

    resultados = []
    for _ in range(2):
        time.sleep(0.01)
        tablero = getBoard()
        counter = np.count_nonzero(tablero) + 1
        resultados.append(counter)
    
    #print_board(tablero)

    return np.mean(resultados)

    
#------------------------------------------------------#


#Funciones Opencv

    
#--------------------------------------------------------#
def getBoard():

    
    # Dimensions of the small matrix (7x6)
    ROW_COUNT = 6
    COLUMN_COUNT = 7

    # Initialize the small matrix with zeros
    board_matrix = np.zeros((ROW_COUNT,COLUMN_COUNT))  

    matrices = []
    for _ in range(100):
        
        time.sleep(0.01)
        
        for col, x in enumerate(vertical_lines):
            mask_x = x

            for row, y in enumerate(horizontal_lines):
                mask_y = y

                #print(f"Entrada a ValueY con centros {mask_y}, {mask_x}")
                valueY = revisar_cuadrado(1, mask_y, mask_x, 3)
                #print(f"Entrada a ValueR con centros {mask_y}, {mask_x}")
                valueR = revisar_cuadrado(2, mask_y, mask_x, 3)
                # valueY = maskY[mask_y, mask_x]
                # valueR = maskR[mask_y, mask_x]     
                
                if valueY == True:
                    board_matrix[row][col] = 1
                elif valueR == True:
                    board_matrix[row][col] = 2
                else:
                    board_matrix[row][col] = 0

                # if board_matrix[row][col] == 1:
                #     color = "Amarillo"
                # elif board_matrix[row][col] == 2:
                #     color = "Rojo"
                # else:
                #     color = "Vacío"               
                    
                #print(f"{row},{col}: {mask_y},{mask_x}; {color} ")
        #for row in board_matrix:
            #print(row)
        matrices.append(board_matrix)
        
        #Media matriz
        media_matriz = np.mean(matrices,axis=0)
        

    return np.flip(media_matriz, 0)

#--------------------------------------------------------#

def revisar_cuadrado(matriz, centro_x, centro_y, lado):
    """
    Revisa si en un cuadrado centrado en las coordenadas dadas
    todos los elementos son iguales a 1.

    Args:
    - matriz (list): La matriz de la imagen.
    - centro_x (int): La coordenada x del centro del cuadrado.
    - centro_y (int): La coordenada y del centro del cuadrado.
    - lado (int): El lado del cuadrado a revisar.

    Returns:
    - bool: True si todos los elementos en el cuadrado son iguales a 1, False de lo contrario.
    """

    # Calcular las coordenadas del cuadrado
    x1 = centro_x - lado // 2
    x2 = centro_x + lado // 2
    y1 = centro_y - lado // 2
    y2 = centro_y + lado // 2

    # Revisar si todos los elementos en el cuadrado son iguales a 1
    if matriz == 1:
        for i in range(x1, x2 + 1):
            for j in range(y1, y2 + 1):
                #print(f"Prueba en punto {i},{j}")
                if maskY[i, j] != 255:
                    return False
        return True
    elif matriz ==2:
        for i in range(x1, x2 + 1):
            for j in range(y1, y2 + 1):
                #print(f"Prueba en punto {i},{j}")
                if maskR[i, j] != 255:
                    return False
        return True
        
#--------------------------------------------------------#


# Funciones MinMax algorithm


#--------------------------------------------------------#
def print_board(board):
	print(np.flip(board, 0))
#--------------------------------------------------------#

def create_board():
	board = np.zeros((ROW_COUNT,COLUMN_COUNT))
	return board

def drop_piece(board, row, col, piece):
	board[row][col] = piece

def is_valid_location(board, col):
	return board[ROW_COUNT-1][col] == 0

def get_next_open_row(board, col):
	for r in range(ROW_COUNT):
		if board[r][col] == 0:
			return r

def winning_move(board, piece):
	# Check horizontal locations for win
	for c in range(COLUMN_COUNT-3):
		for r in range(ROW_COUNT):
			if board[r][c] == piece and board[r][c+1] == piece and board[r][c+2] == piece and board[r][c+3] == piece:
				return True

	# Check vertical locations for win
	for c in range(COLUMN_COUNT):
		for r in range(ROW_COUNT-3):
			if board[r][c] == piece and board[r+1][c] == piece and board[r+2][c] == piece and board[r+3][c] == piece:
				return True

	# Check positively sloped diaganols
	for c in range(COLUMN_COUNT-3):
		for r in range(ROW_COUNT-3):
			if board[r][c] == piece and board[r+1][c+1] == piece and board[r+2][c+2] == piece and board[r+3][c+3] == piece:
				return True

	# Check negatively sloped diaganols
	for c in range(COLUMN_COUNT-3):
		for r in range(3, ROW_COUNT):
			if board[r][c] == piece and board[r-1][c+1] == piece and board[r-2][c+2] == piece and board[r-3][c+3] == piece:
				return True

def evaluate_window(window, piece):
	score = 0
	opp_piece = PLAYER_PIECE
	if piece == PLAYER_PIECE:
		opp_piece = AI_PIECE

	if window.count(piece) == 4:
		score += 100
	elif window.count(piece) == 3 and window.count(EMPTY) == 1:
		score += 5
	elif window.count(piece) == 2 and window.count(EMPTY) == 2:
		score += 2

	if window.count(opp_piece) == 3 and window.count(EMPTY) == 1:
		score -= 4

	return score

def score_position(board, piece):
	score = 0

	## Score center column
	center_array = [int(i) for i in list(board[:, COLUMN_COUNT//2])]
	center_count = center_array.count(piece)
	score += center_count * 3

	## Score Horizontal
	for r in range(ROW_COUNT):
		row_array = [int(i) for i in list(board[r,:])]
		for c in range(COLUMN_COUNT-3):
			window = row_array[c:c+WINDOW_LENGTH]
			score += evaluate_window(window, piece)

	## Score Vertical
	for c in range(COLUMN_COUNT):
		col_array = [int(i) for i in list(board[:,c])]
		for r in range(ROW_COUNT-3):
			window = col_array[r:r+WINDOW_LENGTH]
			score += evaluate_window(window, piece)

	## Score posiive sloped diagonal
	for r in range(ROW_COUNT-3):
		for c in range(COLUMN_COUNT-3):
			window = [board[r+i][c+i] for i in range(WINDOW_LENGTH)]
			score += evaluate_window(window, piece)

	for r in range(ROW_COUNT-3):
		for c in range(COLUMN_COUNT-3):
			window = [board[r+3-i][c+i] for i in range(WINDOW_LENGTH)]
			score += evaluate_window(window, piece)

	return score

def is_terminal_node(board):
	return winning_move(board, PLAYER_PIECE) or winning_move(board, AI_PIECE) or len(get_valid_locations(board)) == 0

def minimax(board, depth, alpha, beta, maximizingPlayer):
	valid_locations = get_valid_locations(board)
	is_terminal = is_terminal_node(board)
	if depth == 0 or is_terminal:
		if is_terminal:
			if winning_move(board, AI_PIECE):
				return (None, 100000000000000)
			elif winning_move(board, PLAYER_PIECE):
				return (None, -10000000000000)
			else: # Game is over, no more valid moves
				return (None, 0)
		else: # Depth is zero
			return (None, score_position(board, AI_PIECE))
	if maximizingPlayer:
		value = -math.inf
		column = random.choice(valid_locations)
		for col in valid_locations:
			row = get_next_open_row(board, col)
			b_copy = board.copy()
			drop_piece(b_copy, row, col, AI_PIECE)
			new_score = minimax(b_copy, depth-1, alpha, beta, False)[1]
			if new_score > value:
				value = new_score
				column = col
			alpha = max(alpha, value)
			if alpha >= beta:
				break
		return column, value

	else: # Minimizing player
		value = math.inf
		column = random.choice(valid_locations)
		for col in valid_locations:
			row = get_next_open_row(board, col)
			b_copy = board.copy()
			drop_piece(b_copy, row, col, PLAYER_PIECE)
			new_score = minimax(b_copy, depth-1, alpha, beta, True)[1]
			if new_score < value:
				value = new_score
				column = col
			beta = min(beta, value)
			if alpha >= beta:
				break
		return column, value

def get_valid_locations(board):
	valid_locations = []
	for col in range(COLUMN_COUNT):
		if is_valid_location(board, col):
			valid_locations.append(col)
	return valid_locations

def pick_best_move(board, piece):

	valid_locations = get_valid_locations(board)
	best_score = -10000
	best_col = random.choice(valid_locations)
	for col in valid_locations:
		row = get_next_open_row(board, col)
		temp_board = board.copy()
		drop_piece(temp_board, row, col, piece)
		score = score_position(temp_board, piece)
		if score > best_score:
			best_score = score
			best_col = col

	return best_col

# def draw_board(board):
# 	for c in range(COLUMN_COUNT):
# 		for r in range(ROW_COUNT):
# 			pygame.draw.rect(screen, BLUE, (c*SQUARESIZE, r*SQUARESIZE+SQUARESIZE, SQUARESIZE, SQUARESIZE))
# 			pygame.draw.circle(screen, BLACK, (int(c*SQUARESIZE+SQUARESIZE/2), int(r*SQUARESIZE+SQUARESIZE+SQUARESIZE/2)), RADIUS)
	
# 	for c in range(COLUMN_COUNT):
# 		for r in range(ROW_COUNT):		
# 			if board[r][c] == PLAYER_PIECE:
# 				pygame.draw.circle(screen, YELLOW, (int(c*SQUARESIZE+SQUARESIZE/2), height-int(r*SQUARESIZE+SQUARESIZE/2)), RADIUS)
# 			elif board[r][c] == AI_PIECE: 
# 				pygame.draw.circle(screen, RED, (int(c*SQUARESIZE+SQUARESIZE/2), height-int(r*SQUARESIZE+SQUARESIZE/2)), RADIUS)
# 	pygame.display.update()

def readBoard():
	return (board)


##-----------------------------------------------

##Inicio del programa

##-----------------------------------------------

#------------------------------------------------------#
#Estados del sistema
#------------------------------------------------------#
test_camaras = 0
calculo = 1
printlineas = 1
startgame = 1

#Decidir cuales cámaras ver
camara_normal = 0
camara_rojos = 0
camara_amarillos = 0
camara_matriz = 0

#------------------------------------------------------#
#Inicio de cámara
#------------------------------------------------------#
camera = cv.VideoCapture(0)
_, frame = camera.read()

for y in horizontal_lines:
    grid1 = cv.line(frame, (0, y), (639, y), (0, 0, 0), 1)

for x in vertical_lines:
    grid2 = cv.line(frame, (x, 0), (x, 479), (0, 0, 0), 1)


#-------------------------------------------------------------#
#Iniciar cámaras ----
#-------------------------------------------------------------#
hsvImage = cv.cvtColor(frame,cv.COLOR_BGR2HSV)
maskR1 = cv.inRange(hsvImage,lowerLimitR1, upperLimitR1)
maskR2 = cv.inRange(hsvImage,lowerLimitR2, upperLimitR2)
maskR = cv.bitwise_or(maskR1,maskR2)
maskY = cv.inRange(hsvImage,lowerLimitY,upperLimitY)
gridTest = cv.add(grid1,grid2)

_, frame = camera.read()
if camara_normal == 1:
    cv.imshow('Camera', frame)
if camara_matriz == 1:
    cv.imshow('Grid', gridTest)
if camara_rojos ==1:
    cv.imshow('Red',maskR)
if camara_amarillos == 1:
    cv.imshow('Yellow',maskY)

while test_camaras:
    cv.imshow('Camera', frame)
    cv.imshow('Grid', gridTest)
    cv.imshow('Red',maskR)
    cv.imshow('Yellow',maskY)
        
    if cv.waitKey(5) & 0xFF == ord('x'):
        break

#------------------------------------------------------#
#Conexión a Arduino
#------------------------------------------------------#
ports = serial.tools.list_ports.comports()
serialInst = serial.Serial()

portsList = []

serialInst.baudrate = 9600
serialInst.port = "COM9"
serialInst.open()
time.sleep(2)
# Setup Arduino
#Communication Test:
command = ("test")
serialInst.write(command.encode('utf-8'))
time.sleep(8)
#------------------------------------------------------#

##----------------------------------------------------##

#Inicio del juego

##----------------------------------------------------##


#Accesorios de inicio

#=======================================================================#
#
#=======================================================================#
while True:
    #=======================================================================#
    if startgame != 0:

        #------------------------------------------------------#
        #Homing al bot
        command = ("homeSteppers")
        serialInst.write(command.encode('utf-8'))
        print("Enviando mensaje de hacer homing")
        time.sleep(35)
        #------------------------------------------------------#

        print("Iniciando sistema...")
        time.sleep(0.2)
        print("Iniciando sistema..")
        time.sleep(0.2)
        print("Iniciando sistema.")
        time.sleep(0.2)
        print("Iniciando sistema...")
        time.sleep(0.2)
        print("Iniciando sistema..")
        time.sleep(0.2)
        print("Iniciando sistema.")
        time.sleep(0.2)
        print("Esperando que se realice el homing")

        time.sleep(20)
        #------------------------------------------------------#
        #Pasar ficha al jugador
        command = ("Iniciojuego")
        serialInst.write(command.encode('utf-8'))
        print("Enviando inicio juego para que pase la primera ficha")
        time.sleep(0.5)
        #------------------------------------------------------#
        


        #Actualizar tablero y revisar turno actual
        board = getBoard()
        turn = getTurn()
        print(f"El turno es: {turn}...from function")

        #Inicio del programa
        print("El juego ha iniciado.")

        print_board(board)
        game_over = True

        #Reinicio de variables
        esperabot = 0
        esperajugador = 0
        turn_recover1 = 1
        turn_recover2 = 1
        badPlay = 0
        # pygame.init()

        SQUARESIZE = 100

        width = COLUMN_COUNT * SQUARESIZE
        height = (ROW_COUNT+1) * SQUARESIZE

        size = (width, height)

        RADIUS = int(SQUARESIZE/2 - 5)

        # screen = pygame.display.set_mode(size)
        # pygame.display.update()

        # myfont = pygame.font.SysFont("monospace", 75)

        
        turn = 1
        if turn == 1:
            print(f"Turno del Jugador")

        startgame = 0   
    #=======================================================================#

    while game_over:
        _, frame = camera.read()
        hsvImage = cv.cvtColor(frame,cv.COLOR_BGR2HSV)
        maskR1 = cv.inRange(hsvImage,lowerLimitR1, upperLimitR1)
        maskR2 = cv.inRange(hsvImage,lowerLimitR2, upperLimitR2)
        maskR = cv.bitwise_or(maskR1,maskR2)
        maskY = cv.inRange(hsvImage,lowerLimitY,upperLimitY)
        for y in horizontal_lines:
            grid1 = cv.line(frame, (0, y), (639, y), (0, 0, 0), 1)
        for x in vertical_lines:
            grid2 = cv.line(frame, (x, 0), (x, 479), (0, 0, 0), 1)
        gridTest = cv.add(grid1,grid2)        
        cv.imshow('Grid', gridTest)
        cv.imshow('Red',maskR)
        cv.imshow('Yellow',maskY)


        # Si el bot falla las fichas 2 veces hace homing
        if badPlay == 2:
            #------------------------------------------------------#
            #Homing al bot
            command = ("homeSteppers")
            serialInst.write(command.encode('utf-8'))
            print("Enviando mensaje de hacer homing... Dos errores consecutivos")
            time.sleep(60)
            badPlay = 0
            #------------------------------------------------------#
            
        #Tomar el turno al empezar y el tablero actual
        board = getBoard()
        turn = getTurn()


        #Check if someone wins
        if winning_move(board, PLAYER_PIECE):
            game_over = False
            esperabot = 1
            esperajugador = 1
            print(f"Jugador 1 Ha ganado")

        if winning_move(board, AI_PIECE):
            game_over = False
            esperabot = 1
            esperajugador = 1
            print(f"IA Ha ganado")
            command = "Bailecito"
            serialInst.write(command.encode('utf-8'))
            time.sleep(0.05)
        
        if turn % 2 != PLAYER:
            time.sleep(0.05)
            print(f"El turno es: {turn}")
            
            if esperajugador == 0:
                time.sleep(2)
                esperabot = 0
                print(f"Turno del jugador, esperar a que el jugador juegue...")
                print_board(board)
                esperajugador = 1


        # # Ask for Player 2 Input
        if turn % 2 != AI and game_over:
            time.sleep(0.05)
            print(f"El turno es: {turn}")
                
            if esperabot == 0:
                esperajugador = 0
                print(f"Turno de la IA, esperar a que la IA juegue...")
                print_board(board)
                #Aquí es donde la IA calcula cuál es el mejor movimiento, así que antes se deberá de actualizar el tablero.
                col, minimax_score = minimax(board, 5, -math.inf, math.inf, True)
                print(f"La mejor jugada es {col+1}")
                if col == 0:
                    move = ("Posicion1")
                    print("Enviando Arduino a Posicion1")
                elif col == 1:
                    move = ("Posicion2")
                    print("Enviando Arduino a Posicion2")
                elif col ==2:
                    move = ("Posicion3")
                    print("Enviando Arduino a Posicion3")
                elif col ==3:
                    move = ("Posicion4")
                    print("Enviando Arduino a Posicion4")
                elif col ==4:
                    move = ("Posicion5")
                    print("Enviando Arduino a Posicion5")
                elif col ==5:
                    move = ("Posicion6")
                    print("Enviando Arduino a Posicion6")
                elif col ==6:
                    move = ("Posicion7")
                    print("Enviando Arduino a Posicion7")
                    
                command = move
                serialInst.write(command.encode('utf-8'))
                time.sleep(0.05)
                
                print(f"Mensaje {move} enviado al Arduino")   

                #Tomamos el tiempo en el que jugó el robot por si falla la jugada
                robot_time = current_milli_time()
                
                #Espera bot se utiliza para que no envíe el robot nuevamente a moverse
                esperabot = 1 
                #----------------------------------------------------------------------//
                
            current_time = current_milli_time()

            #Si es el turno del robot y la ficha no ha caído en 40s, jugará nuevamente.
            if current_time > robot_time + 40000:
                print(f"Se ha agotado el tiempo de espera para el robot, enviando jugada nuevamente...")
                badPlay=badPlay+1
                print(f"El Robot lleva {badPlay} jugadas falladas... Cuando llegue a 2 hace Homing.")
                esperabot = 0

                #----------------------------------------------------------------------//
                                
            

                
        if cv.waitKey(5) & 0xFF == ord('q'):
            break

    

    if game_over == False:
        answer = input(f"El juego se ha terminado, Si desea jugar nuevamente pulse 1 \n")
        if answer == "1":
            startgame = 1
            game_over = True
        else:
            camera.release()
            cv.destroyAllWindows()
            

    


#=======================================================================#
#
#=======================================================================#

    

  