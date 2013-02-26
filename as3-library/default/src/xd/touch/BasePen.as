package xd.touch
{
	import com.senocular.ui.VirtualMouse;
	
	import flash.desktop.NativeApplication;
	import flash.display.NativeWindow;
	import flash.events.MouseEvent;
	import flash.events.TouchEvent;
	import flash.geom.Point;

	public class BasePen extends BaseInput
	{
		public var pressure:Number;
		public var buttons:uint;
		protected var lastButtons:uint;
		/** while not the most elegant way of doing things, this allows for EASY prototyping of behaviors, which is really what we're after. */
		private static const PEN_ID:int = 99999;
		private static const ERASER_ID:int = 88888;
		
		public function BasePen()
		{
			alsoSendsAsMouse = true;
		}
		public function toString():String {
			var a:Array = ['xd.touch.Pen',onAirDesktop.x.toFixed(2), onAirDesktop.y.toFixed(2), pressure.toFixed(2), buttons.toString(16)];
			return a.join(' ');
		}
		

		protected function get eventID():int { return PEN_ID;} 
		public override function dispatch():void {
		mapToStage();
		// no need to call this, since pen events ALWAYS go out as mouse events
		//	super.dispatch();
			
			// todo: move this into read
			var touchType:String = lastButtons ? ((buttons&1) ? TouchEvent.TOUCH_MOVE : TouchEvent.TOUCH_END) 
				: ((buttons&1) ? TouchEvent.TOUCH_BEGIN : "proximityMove"); 
			
			lastButtons = (buttons & 1);
			
			dist_te(touchType, inActiveWindow, eventID,_undefinedPoint, pressure, intent, true,buttons);
			
//			var mouseType:String = lastButtons ? (buttons ? MouseEvent. : TouchEvent.TOUCH_END) 
//				: (buttons ? TouchEvent.TOUCH_BEGIN : PenEvent.PEN_HOVER); 
//			
			
		}
		
		
	}
}