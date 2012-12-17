package xd.touch.wacom
{
	import flash.events.TouchEvent;
	
	import xd.touch.BaseContact;
	
	public class WacomContact extends BaseContact
	{
		public var sensitivity:uint;
		public var confidence:Boolean;
		public var orientation:Number;
		public var state:int;
		
		private static const kTypeMap:Vector.<String> = new <String>['none',TouchEvent.TOUCH_BEGIN, TouchEvent.TOUCH_MOVE, TouchEvent.TOUCH_END];

		private const kTouchTypeMap:Vector.<String> = Vector.<String>([
			TouchEvent.TOUCH_BEGIN, TouchEvent.TOUCH_MOVE, TouchEvent.TOUCH_END, null]);
		
		public override function dispatch():void {
			touchType = kTypeMap[state];
		//	trace(id, touchType);
			super.dispatch();
		}
		
	}
}