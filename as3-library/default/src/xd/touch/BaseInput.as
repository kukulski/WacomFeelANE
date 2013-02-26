package xd.touch
{
	import com.senocular.ui.VirtualMouse;
	
	import flash.desktop.NativeApplication;
	import flash.display.DisplayObject;
	import flash.display.InteractiveObject;
	import flash.display.NativeWindow;
	import flash.display.Stage;
	import flash.display.StageDisplayState;
	import flash.events.TouchEvent;
	import flash.events.TouchEventIntent;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.system.ApplicationDomain;

	public class BaseInput
	{
		public var onAirDesktop:Point = new Point;
		public var inActiveWindow:Point = new Point;
		public var alsoSendsAsMouse:Boolean = false;
		public var down:Boolean;
		public var touchType:String;
		public var intent:String = TouchEventIntent.UNKNOWN;
		
		protected var _stage:Stage;
		
		internal static var _stopped:Boolean;
		
		protected static const _undefinedPoint:Point = new Point(undefined, undefined);
		
		public function BaseInput()
		{
		}

		public function dispatch():void {

		}
		
		private function dispatchFancy(onStage:Point, makeEvent:Function):void {

			var targets:Array = _stage.getObjectsUnderPoint(onStage);
			targets.reverse();
			targets.push(_stage);
			
			_stopped = false;
			
			for each (var target:DisplayObject in targets) {
				var iObj:InteractiveObject = target as InteractiveObject;
				dispatchTo(iObj, makeEvent);
				if(_stopped) break;
			}
		}
		
		private function dispatchTo(iTarget:InteractiveObject, makeEvent:Function):Boolean {
			
			while(iTarget) {						// climb up from the hit object to an interactiveObject
				
				if(iTarget)
				{
					var event:TouchEvent = makeEvent(iTarget);
					
					return iTarget.dispatchEvent(event); 
				}
				iTarget = iTarget.parent;	
			}
			return true;
		}
		
		protected function dist_te(type:String, onStage:Point, id:int, sz:Point, pr:Number, intent:String = TouchEventIntent.UNKNOWN, isPrimary:Boolean = false, buttons:int=0):TouchEvent {
			
			var makeEvent:Function = function(target:InteractiveObject):TouchEvent{
				var local:Point = target.globalToLocal(onStage);
				var event:PenEvent = new PenEvent(type,true,false,id, isPrimary, local.x, local.y, sz.x, sz.y, pr,target,false,false,false,false,false,NaN,intent,buttons);
				return event;
				};
			
			
			var overlay:ITouchOverlay = DeviceManager.instance.overlay;
				
			if(overlay) {
				overlay.unhook();
			}
		
			
			dispatchFancy(onStage, makeEvent);
			

			if(type == TouchEvent.TOUCH_END) {
				dist_te(TouchEvent.TOUCH_TAP, onStage, id, sz, pr, intent, isPrimary);
			}
			
			if(overlay) {
				overlay.update(_stage, type, onStage.x, onStage.y, id);
			}
			return null;
		}
		

		
		private static const _pzz:Point = new Point(0,0);
		
		public function mapToStage():void {
			var win:NativeWindow = NativeApplication.nativeApplication.activeWindow;
			if(!win) 
				win = NativeApplication.nativeApplication.openedWindows[0] as NativeWindow;
			
			_stage = win.stage;
			toWindowInPlace(onAirDesktop, win, inActiveWindow);
		}
		

		private function toWindowInPlace(worldPoint:Point, window:NativeWindow, into:Point):void {
			if(!window) return;
			var zz_onscreen:Point = window.globalToScreen(_pzz);
			

				into.x = worldPoint.x - zz_onscreen.x;
				into.y = worldPoint.y - zz_onscreen.y;

				var stage:Stage = window.stage;
				
				if(stage.displayState == StageDisplayState.FULL_SCREEN_INTERACTIVE) {
					var rect:Rectangle = stage.fullScreenSourceRect || new Rectangle(0,0,stage.stageWidth, stage.stageHeight);
					var scale:Number = rect.width/stage.fullScreenWidth;
					
					//TODO: fix this to work with arbitrary fullscreenSourceRect -- current assumes 0,0 origin
					
					into.x *= scale;
					into.y -= .5 * (stage.fullScreenHeight - (rect.height / scale));
					into.y *= scale;
				} 
			
		}
//		
//		//TODO: split this into findStage / activateStage  & move into the device manager
//		protected function findStage(worldPoint:Point):NativeWindow {
//			
//			
//			return NativeApplication.nativeApplication.openedWindows[0] as NativeWindow;
//			
////			/* this chunk of code can probably find a better place to live --
////			
////			the idea here is that a touch through the driver stack should activate the running application.
////			
////			it doesn't pay attention to window order
////			and we don't handle things like close buttons and resizing
////			
////			*/
////			
////			var activeWindow:NativeWindow = NativeApplication.nativeApplication.activeWindow;
////			if(!alsoSendsAsMouse && !activeWindow) {
////				var windows:Vector.<NativeWindow> = DeviceManager.instance.windows;
////				for each (var w:NativeWindow in windows) {
////					var bounds:Rectangle = w.bounds;
////					if(bounds.containsPoint(worldPoint)) {
////						activeWindow = w;
////						NativeApplication.nativeApplication.activate(activeWindow);
////						activeWindow.orderToFront();
////						break;
////					} // if matches
////				}// for each window
////				
////			}// if no active window
////
////			_stage = activeWindow ? activeWindow.stage : null;
////			return activeWindow;
//		}
//		
	}
}