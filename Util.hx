class Util {
        public static function cellToPixelX(x: Int): Float {
                return x * Defs.BOARD_CELL_W;
        }

        public static function cellToPixelY(y: Int): Float {
                return y * Defs.BOARD_CELL_H;
        }

        public static function pixelToCellX(x: Float): Int {
                return Std.int(x / Defs.BOARD_CELL_W);
        }

        public static function pixelToCellY(y: Float): Int {
                return Std.int(y / Defs.BOARD_CELL_W);
        }

        public static function pixelToCellPos(x: Float, y: Float): Int {
                var cellX = pixelToCellX(x);
                var cellY = pixelToCellY(y);

                return cellY * Defs.BOARD_W + cellX;
        }

        public static function cellPosToCellX(pos: Int): Int {
                return pos % Defs.BOARD_W;
        }

        public static function cellPosToCellY(pos: Int): Int {
                return Std.int(pos / Defs.BOARD_W);
        }
}
