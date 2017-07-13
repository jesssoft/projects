/*
 * Snake's direction
 */
enum Dir {
        DIR_NONE;
        DIR_UP;
        DIR_DOWN;
        DIR_RIGHT;
        DIR_LEFT;
}

/*
 * Board cell index
 */
enum CellIndex {
        CI_EMPTY;
        CI_SNAKE;
        CI_SNAKE_HIT;
}

/*
 * Constants
 */
class Defs {
        /*
         * Frames
         */
        public static var FPS(default, never): Float = 2; // Increasing means speedup
        public static var SPF(default, never): Float = 1/FPS;

        /*
         * Board
         */
        public static var BOARD_W(default, never): Int = 30;
        public static var BOARD_H(default, never): Int = 30;
        public static var BOARD_CELL_W(default, never): Int = 20;
        public static var BOARD_CELL_H(default, never): Int = 20;
        public static var BOARD_CELL_COUNT(default, never): Int = BOARD_W * BOARD_H;
        public static var BOARD_CENTER_X(default, never): Int = Std.int(BOARD_W/2);
        public static var BOARD_CENTER_Y(default, never): Int = Std.int(BOARD_H/2);

        /*
         * Snake
         */
        public static var DEFAULT_SNAKE_BODY_COUNT(default, never): Int = 3;
}
