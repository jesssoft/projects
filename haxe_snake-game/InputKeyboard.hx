import flash.events.KeyboardEvent;
import Defs;

class InputKeyboard implements Input {
        private var _keys = new Map<UInt, Dir>();
        private var _dir = DIR_NONE;

        public function getDir(): Dir {
                return _dir;
        }

        public function new() {
                // Nothing to do
        }

        public function addKeyPair(key: UInt, dir: Dir): Void {
                _keys.set(key, dir);
        }

        public function onKeyDown(event: KeyboardEvent): Bool {
                if (!_keys.exists(event.keyCode))
                        return false;

                _dir = _keys[event.keyCode];
                return true;
        }
}
