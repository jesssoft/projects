import flash.Lib;
import flash.display.Shape;
import flash.display.Sprite;
import flash.ui.Keyboard;
import flash.events.KeyboardEvent;
import flash.events.Event;
import haxe.Timer;
import Defs;

class Main extends Sprite {
        /*
         * Static variables
         */
        static var _stage = Lib.current.stage;

        /*
         * Member variables
         */
        var _prevTime: Float = 0;
        var _accumTime: Float = 0;
        var _apple: Apple = null;
        var _board = new Array<CellIndex>();
        var _hitPos = new List<Int>();
        var _snakes = new List<Snake>();

        /*
         * Member functions
         */
        static function main() {
                _stage.addChild(new Main());
        }

        public function new() {
                super();
                init();
                startGame();
        }

        private function init(): Void {
                /*
                 * Add evnets we need in this game
                 */
                 initEvents();

                /*
                 * Display the board area
                 */
                initBackground();
        }

        private function initEvents(): Void {
                _stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
                _stage.addEventListener(Event.ENTER_FRAME, onUpdateFrame);
        }

        private function initBackground(): Void {
                var shape = new Shape();

                shape.graphics.beginFill(0xf0f0f0);
                shape.graphics.drawRect(0, 0,
                        Defs.BOARD_W * Defs.BOARD_CELL_W,
                        Defs.BOARD_H * Defs.BOARD_CELL_H);
                shape.graphics.endFill();

                shape.x = 0;
                shape.y = 0;

                _stage.addChild(shape);
        }

        private function cleanup(): Void {
                /*
                 * Cleanup apple
                 */
                if (_apple != null) {
                        _apple.delete();
                        _apple = null;
                }

                /*
                 * Cleanup all snakes
                 */
                for (snake in _snakes) {
                        snake.delete();
                }
                _snakes.clear();

                /*
                 * Cleanup the hit postions
                 */
                _hitPos.clear();
        }

        private function startGame(): Void {
                /*
                 * When the game ends, stargGame() will be called again so
                 * cleanup() function should be called here.
                 */
                cleanup();

                /*
                 * Set values
                 */
                _prevTime = Timer.stamp();
                _accumTime = 0;

                /*
                 * Clear the board
                 */
                resetBoard();

                /*
                 * Player1
                 */
                var snake = new Snake(Defs.BOARD_CENTER_X + Std.int(Defs.BOARD_CENTER_X/2),
                                        Defs.BOARD_CENTER_Y);
                // Key setting
                snake.getKeyboard().addKeyPair(Keyboard.UP, DIR_UP);
                snake.getKeyboard().addKeyPair(Keyboard.DOWN, DIR_DOWN);
                snake.getKeyboard().addKeyPair(Keyboard.RIGHT, DIR_RIGHT);
                snake.getKeyboard().addKeyPair(Keyboard.LEFT, DIR_LEFT);

                // For finding out where the hit position is
                snake.setFootprint(_board);

                // Add to the list
                _snakes.add(snake);

                /*
                 * Player2
                 */
                snake = new Snake(Defs.BOARD_CENTER_X - Std.int(Defs.BOARD_CENTER_X/2),
                                        Defs.BOARD_CENTER_Y);

                // Key setting
                snake.getKeyboard().addKeyPair(Keyboard.W, DIR_UP);
                snake.getKeyboard().addKeyPair(Keyboard.S, DIR_DOWN);
                snake.getKeyboard().addKeyPair(Keyboard.D, DIR_RIGHT);
                snake.getKeyboard().addKeyPair(Keyboard.A, DIR_LEFT);

                // For finding out where the hit position is
                snake.setFootprint(_board);

                // Add to the list
                _snakes.add(snake);

                /*
                 * Create an apple on the board. This function uses the footpints
                 * to find out where the empty positions are on the board, so
                 * this function should be called after all the setFootprint calls.
                 */
                spawnApple();
        }

        private function spawnApple(): Void {
                /*
                 * If there is an apple, we don't need to create another one.
                 */
                if (_apple != null)
                        return;

                /*
                 * Calculate the count of empty cells and if it is zero, we cannot
                 * create an apple.
                 */
                var emptyCellCount = getEmptyCellCount();
                if (emptyCellCount <= 0)
                        return;

                /*
                 * Decide where the apple will appear
                 */
                var randomVal = Std.random(emptyCellCount);
                var applePos = calcCellPos(randomVal);
                if (applePos < 0)
                        return;

                /*
                 * Convert the apple position to cell x, y.
                 */
                var appleCellX = Util.cellPosToCellX(applePos);
                var appleCellY = Util.cellPosToCellY(applePos);

                /*
                 * Finally, create an apple.
                 */
                _apple = new Apple(appleCellX, appleCellY);
        }

        private function calcCellPos(randomVal: Int) {
                /*
                 * Using the randomVal, try to find the location.
                 */
                for (i in 0...Defs.BOARD_CELL_COUNT) {
                        if (_board[i] != CI_EMPTY)
                                continue;

                        randomVal--;
                        if (randomVal <= 0)
                                return i;
                }

                return -1;
        }

        private function resetBoard(): Void {
                 for (i in 0...Defs.BOARD_CELL_COUNT) {
                         _board[i] = CI_EMPTY;
                 }
        }

        private function getEmptyCellCount(): Int {
                var count: Int = 0;

                for (i in 0...Defs.BOARD_CELL_COUNT) {
                        if (_board[i] == CI_EMPTY)
                                count++;
                }

                return count;
        }

        private function collectHitPosFromBoard(): Void {
                _hitPos.clear();

                for (i in 0...Defs.BOARD_CELL_COUNT) {
                        if (_board[i] == CI_SNAKE_HIT) {
                                _hitPos.add(i);
                        }
                }
        }

        private function onUpdateFrame(event: Event): Void {
                var now = Timer.stamp();

                _accumTime += now - _prevTime;
                _prevTime = now;
                if (_accumTime < Defs.SPF)
                        return;

                _accumTime -= Defs.SPF;

                onFrame();
        }

        private function onFrame_updateAllSnakes() {
                for (snake in _snakes)
                        snake.onUpdateFrame();
        }

        private function onFrame_removeAllSnakesIfOutOfBoard() {
                for (snake in _snakes) {
                        if (snake.isOutOfBoard()) {
                                snake.delete();
                                _snakes.remove(snake);
                        }
                }
        }

        private function onFrame_updateAllFootprints() {
                for (snake in _snakes)
                        snake.setFootprint(_board);
        }

        private function onFrame_removeAllSnakesIfHit() {
                for (snake in _snakes) {
                        for (hitPos in _hitPos) {
                                if (hitPos == snake.getHeadCellPos()) {
                                        snake.delete();
                                        _snakes.remove(snake);
                                        break;
                                }
                        }
                }
        }

        private function onFrame_apple() {
                if (_apple != null) {
                        for (snake in _snakes) {
                                if (snake.getHeadCellPos() == _apple.getCellPos()) {
                                        snake.eatApple();
                                        _apple.delete();

                                        /*
                                         * The apple will be created in the next frame
                                         */
                                        _apple = null;
                                        break;
                                }
                        }
                } else
                        spawnApple();
        }

        private function onFrame(): Void {
                /*
                 * Clear the board
                 */
                resetBoard();

                /*
                 * First of all, update all the snakes
                 */
                onFrame_updateAllSnakes();

                /*
                 * Remove the snakes which have gone out of the board
                 */
                onFrame_removeAllSnakesIfOutOfBoard();

                /*
                 * If all players are dead, then restart the game
                 */
                if (_snakes.length == 0) {
                        startGame();
                        return;
                }

                /*
                 * Need to call this function for the collision checking
                 */
                onFrame_updateAllFootprints();

                /*
                 * collectHitPosFromBoard() should be called after updating
                 * footprints. This data will be used inside
                 * onFrame_removeAllSnakesIfHit() call.
                 */
                collectHitPosFromBoard();

                /*
                 * When a snake hit itself or the other body
                 */
                onFrame_removeAllSnakesIfHit();

                /*
                 * If all players are dead, then restart the game
                 */
                if (_snakes.length == 0) {
                        startGame();
                        return;
                }

                /*
                 * Any snake can have the apple?
                 */
                onFrame_apple();
        }

        private function onKeyDown(event: KeyboardEvent): Void {
                switch (event.keyCode) {
                /* Just for testing
                case Keyboard.R:
                        startGame();
                */
                default:
                        for (snake in _snakes) {
                                if (snake.getKeyboard().onKeyDown(event))
                                        break;
                        }
                }
        }
}
