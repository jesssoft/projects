import flash.Lib;
import flash.display.Sprite;
import flash.display.Shape;
import Defs;

class Apple {
        /*
         * Static variables
         */
        private static var _stage = Lib.current.stage;

        /*
         * Member variables
         */
        private var _apple: Shape = null;

        public function new(x: Int, y: Int) {
                _apple = new Shape();

                _apple.graphics.beginFill(0xff0000);
                _apple.graphics.drawRect(0, 0,
                        Defs.BOARD_CELL_W, Defs.BOARD_CELL_H);
                _apple.graphics.endFill();

                _apple.x = Util.cellToPixelX(x);
                _apple.y = Util.cellToPixelY(y);

                _stage.addChild(_apple);
        }

        public function getCellPos(): Int {
                var cellPos = Util.pixelToCellPos(_apple.x, _apple.y);

                return cellPos;
        }

        public function delete(): Void {
                _stage.removeChild(_apple);
        }
}
