package xd.touch
{

	//import flash.events.EventDispatcher;
	import flash.events.StatusEvent;
	import flash.external.ExtensionContext;
	import flash.text.TextField;

	public class WacomANE  // extends EventDispatcher
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
			if(tf) tf.appendText(event.toString());
			trace(event);
		}
		public function init():void {
			
			
			trace(_ExtensionContext.toString());
			trace(_ExtensionContext.call("init"));
		}
		
		
		
	}
}