package xd.touch
{

	//import flash.events.EventDispatcher;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.StatusEvent;
	import flash.external.ExtensionContext;
	import flash.text.TextField;

	public class WacomANE  extends EventDispatcher
	{
		private var   _ExtensionContext:ExtensionContext;

		public function WacomANE()
		{
			try
			{
				_ExtensionContext = ExtensionContext.createExtensionContext("xd.touch.WacomANE", null);
				_ExtensionContext.addEventListener(StatusEvent.STATUS, gotEvent);				
				trace("wacom ANE initialized (and this is the updated one)");
			}
			catch (e:Error)
			{
				trace(e);
			}
			
		}
		
		public var tf:TextField; 
		private function gotEvent(event:StatusEvent):void
		{
			trace(event);
			
			if(tf) tf.appendText(event.toString());
			dispatchEvent(new Event(event.code));
			trace(event);
		}
		
		public function sendEvent(type:String):void {
			_ExtensionContext.call("sendEvent",type);
		}
		
		public function init():* {
			return(_ExtensionContext.call("init"));
		}
		
		
		
	}
}