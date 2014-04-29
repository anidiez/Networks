Welcome to battleship.

Servers expect to receive hits and ships from the client. Clients expect to receive game data from the server. Both handle errors.

The expected packet formatting for each of these packets is as follows:

SHIP   1 byte    1 byte   string    string
      --------------------------------------------
      | Opcode |  type  | position | orientation |
      --------------------------------------------
With values as follow:
  Opcode should be 2.
  Type should be a number between 2 and 5 which reflects the length of the ship.
  Position should be in the form of XY where X is a letter from A-J and Y is a number from 0-9
  Orientation should be either "vertical" or "horizontal"

HIT   1 byte     string
      ---------------------
      | Opcode | position |
      ---------------------
With values as follow
  Opcode should be 1.
  posittion should be in the form of XY where X is a letter from A-J and Y is a number from 0-9


