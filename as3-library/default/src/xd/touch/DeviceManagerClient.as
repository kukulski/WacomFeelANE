package xd.touch
{
	import flash.display.Screen;
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageDisplayState;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.geom.Rectangle;
	
	import xd.util.ScreenUtils;
	
	
	public class DeviceManagerClient extends Sprite
	{
		protected var _devMgr:DeviceManager = DeviceManager.instance;
		
		
		
		public function DeviceManagerClient()
		{
			
			addEventListener(Event.ADDED_TO_STAGE, privateInit);
		}

		
		private function setBounds(e:*):void {
			fixBounds(DeviceManager.instance.bounds);
			
			stage.nativeWindow.bounds = DeviceManager.instance.bounds; 
		}
		private function privateInit(e:Event):void {
			removeEventListener(Event.ADDED_TO_STAGE, privateInit);
			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			DeviceManager.instance.addEventListener("Bounds",setBounds);
			DeviceManager.instance.registerWindow(stage.nativeWindow);

			
			var b:Rectangle = DeviceManager.instance.bounds;
			
			
			
			
			if(b) {
				fixBounds(b);
				
				stage.nativeWindow.bounds = b; 
				if(DeviceManager.instance.fullscreen)
					stage.displayState = StageDisplayState.FULL_SCREEN_INTERACTIVE;
			}
			
				
				clientInit(e);
			stage.nativeWindow.visible = true;
		}
		
		private function fixBounds(b:Rectangle):void {
			
			var screens:Array = Screen.getScreensForRectangle(b);
			if(screens.length == 1) {
				var screen:Screen = screens[0];
				if(screen.bounds.width != b.width || screen.bounds.height != b.height) {
					useReplacementScreen(b);
				}
					
			}
			
		}
		private function useReplacementScreen(b:Rectangle):void {
			var rightSizedScreen:Screen = ScreenUtils.find(b.width, b.height);
			b.x = rightSizedScreen.bounds.x;
			b.y = rightSizedScreen.bounds.y;
		}
		

		
		
		protected function clientInit(e:Event):void {
			
		}
		
	}
}