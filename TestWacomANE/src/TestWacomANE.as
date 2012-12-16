package
{
	import flash.display.Sprite;
	import flash.events.Event;
	
	import xd.touch.WacomANE;
	
	public class TestWacomANE extends Sprite
	{
		public function TestWacomANE()
		{
			var wacom:WacomANE = new WacomANE;
			trace(wacom.init());
			wacom.addEventListener("aMousefl",onFoo);
			wacom.addEventListener("what",onFoo);
			wacom.sendEvent("what");
			
			
		}
		private function onFoo(e:Event):void {
			trace("onFoo",e.type);
			
			
		}
	}
}