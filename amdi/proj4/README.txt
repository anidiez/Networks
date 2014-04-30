Welcome to battleship.

Servers expect to receive hits and ships from the client. Clients expect to receive game data from the server. Both handle errors.

The expected packet formatting for each of these packets is as follows:

HIT   1 byte     string
      ---------------------
      | Opcode | position |
      ---------------------
With values as follow
  Opcode should be 1.
  position - we should figure out how to express this

SHIP   1 byte    1 byte   string    1 byte  string
      ----------------------------------------------------
      | Opcode |  type  | position |   ;   | orientation |
      ----------------------------------------------------
With values as follow:
  Opcode should be 2.
  Type should be a number between 2 and 5 which reflects the length of the ship.
  Position - we should figure out how to express this
  Orientation should be either "vertical" or "horizontal"

GAME_DATA 1 byte    string        1 byte  string
          ------------------------------------------
          | Opcode | player board | ; | player hits|
          ------------------------------------------
  Opcode should be 3
  player board should be string of characters representing the current board
  ; for separating boards
  player htis should be string of characters representing the hits made (and if they were successful)

ACK   1 byte    1 byte
      --------------------------------
      | Opcode | Confirmation Opcode |
      --------------------------------
  Opcode should be 4
  server confirms receipt of client opcode by echoing it

ERROR 1 byte   string
      ------------------------
      |Opcode | error message|
      ------------------------
  Opcode should be 5
  error message is an error message (i know, shocking) 
