package xd.touch.wacom
{
	import flash.events.TouchEventIntent;
	import flash.geom.Point;
	
	import xd.touch.BasePen;

	public class PenInput extends BasePen
	{
		public var type:int;
		public var tablet:int;
		public var tool:int;
		public var z:int;
		public var tilt:Point = new Point;
		public var tangentialPressure:Number;
		public var rotation:Number;

		public function adjust(y):void {
			intent = tool == 1 ? TouchEventIntent.PEN : TouchEventIntent.ERASER;
		}	
		
	}
}