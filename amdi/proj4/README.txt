Welcome to battleship.

Servers expect to receive hits and ships from the client. Clients expect to receive game data from the server. Both handle errors.

The expected packet formatting for each of these packets is as follows:

HIT   1 byte     2 bytes
      ---------------------
      | Opcode | position |
      ---------------------
With values as follow
  Opcode should be 1.
  position - we should figure out how to express this

SHIP   1 byte    1 byte   2 bytes    1 byte  1 byte
      ----------------------------------------------------
      | Opcode |  type  | position |   ;   | orientation |
      ----------------------------------------------------
With values as follow:
  Opcode should be 2.
  Type should be (char or number, not sure which).
  Position - 2 bytes - number from 0 to 99 to match array position
  Orientation should be either 0 for down and 1 for right

GAME_DATA 1 byte    2 bytes    1 byte    1 byte
          -------------------------------------------
          | Opcode | position |   ;   | fail/success|
          -------------------------------------------
  Opcode should be 3
  

ACK   1 byte    1 byte
      --------------------------------
      | Opcode | Confirmation Opcode |
      --------------------------------
  Opcode should be 4
  server confirms receipt of client opcode by echoing it

TURN 1byte  1byte
     ---------------------
     | Opcode | turn info|
     ---------------------
  Opcode shhould be 5
  turn info is 1 if it's that player's turn, 0 if it's not

ERROR 1 byte   string
      ------------------------
      |Opcode | error message|
      ------------------------
  Opcode should be 6
  error message is an error message (i know, shocking) 
