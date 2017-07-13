import flash.Lib;
import flash.display.Sprite;
import flash.display.Shape;
import Defs;

class Snake {
        /*
         * Static variables
         */
        private static var _stage = Lib.current.stage;

        /*
         * Member variables
         */
        private var _head: Shape = null;
        private var _dir = DIR_UP;
        private var _appleCount = 0;
        private var _bodies = new List<Shape>();
        private var _input = new InputKeyboard();

        public function new(x: Int, y: Int) {
                /*
                 * Build a snake's body
                 */
                for (i in 0...Defs.DEFAULT_SNAKE_BODY_COUNT) {
                        var nx = Util.cellToPixelX(x);
                        var ny = Util.cellToPixelY(y + i);

                        addBody(nx, ny);
                }
        }

        public function getKeyboard(): InputKeyboard {
                return _input;
        }

        private function addBody(x: Float, y: Float): Void {
                var shape = new Shape();

                shape.graphics.beginFill(0x000000);
                shape.graphics.drawRect(0, 0, Defs.BOARD_CELL_W, Defs.BOARD_CELL_H);
                shape.graphics.endFill();

                shape.x = x;
                shape.y = y;

                _bodies.add(shape);
                _stage.addChild(shape);

                if (_head == null)
                        _head = shape;
        }

        public function delete(): Void {
                for (item in _bodies)
                        _stage.removeChild(item);
        }

        public function onUpdateFrame(): Void {
                /*
                 * Current snake head position
                 */
                var nx = _head.x;
                var ny = _head.y;

                /*
                 * Update keyboard code
                 */
                var dir = _input.getDir();
                setDir(dir);

                /*
                 * Calculate the new head position
                 */
                switch (_dir) {
                case DIR_UP:
                        nx += 0;
                        ny += -Defs.BOARD_CELL_H;
                case DIR_DOWN:
                        nx += 0;
                        ny += Defs.BOARD_CELL_H;
                case DIR_RIGHT:
                        nx += Defs.BOARD_CELL_W;
                        ny += 0;
                case DIR_LEFT:
                        nx += -Defs.BOARD_CELL_W;
                        ny += 0;
                default:
                }

                /*
                 * Move all snake's bodies
                 */
                var tempx: Float = 0;
                var tempy: Float = 0;

                for (body in _bodies) {
                        tempx = body.x;
                        tempy = body.y;

                        body.x = nx;
                        body.y = ny;

                        nx = tempx;
                        ny = tempy;
                }

                /*
                 * If this snake had an apple, create one body cell
                 */
                if (_appleCount > 0) {
                        _appleCount--;
                        addBody(nx, ny);
                }
        }

        private function setDir(dir: Dir): Void {
                if (dir == DIR_NONE)
                        return;

                /*
                 * Every direction cannot turn 180
                 */
                switch (_dir) {
                case DIR_UP:
                        if (dir != DIR_DOWN)
                                _dir = dir;
                case DIR_DOWN:
                        if (dir != DIR_UP)
                                _dir = dir;
                case DIR_RIGHT:
                        if (dir != DIR_LEFT)
                                _dir = dir;
                case DIR_LEFT:
                        if (dir != DIR_RIGHT)
                                _dir = dir;
                default:
                        // Nothing to do
                }
        }

        public function setFootprint(board: Array<CellIndex>): Void {
                for (body in _bodies) {
                        var cellPos = Util.pixelToCellPos(body.x, body.y);
                        var cellIndex = board[cellPos];

                        /*
                         * If there is already CI_SNAKE index, which means collision, so
                         * make it CI_SNKE_HIT. If any sanke's head position is here,
                         * they will die.
                         */
                        if (cellIndex == CI_EMPTY)
                                board[cellPos] = CI_SNAKE;
                        else if (cellIndex == CI_SNAKE)
                                board[cellPos] = CI_SNAKE_HIT;
                }
        }

        public function getHeadCellPos(): Int {
                var cellPos = Util.pixelToCellPos(_head.x, _head.y);

                return cellPos;
        }

        public function eatApple(): Void {
                /*
                 * Just increase the count, which will be handled in onUpdateFrame() call.
                 */
                _appleCount++;
        }

        public function isOutOfBoard(): Bool {
                var cellX = Util.pixelToCellX(_head.x);
                var cellY = Util.pixelToCellY(_head.y);

                if (cellX < 0 || cellX >= Defs.BOARD_W)
                        return true;
                if (cellY < 0 || cellY >= Defs.BOARD_H)
                        return true;

                return false;
        }
}
