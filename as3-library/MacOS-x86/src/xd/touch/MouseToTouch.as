package xd.touch
{
	import flash.display.DisplayObject;
	import flash.display.InteractiveObject;
	import flash.display.Stage;
	import flash.events.MouseEvent;
	import flash.events.TouchEvent;
	import flash.geom.Point;
	import flash.utils.Dictionary;

	
	
	/**  example usage: 
	 * 
	 * public class MouseUI extends Collage
	 * {
	 *	private var mtt:MouseToTouch;
	 *
	 *		protected override function init(e:Event):void {
	 *		super.init(e);
	 *		mtt = new MouseToTouch(stage);
	 *	}
	 * */
	public class MouseToTouch
	{
		private var _stage:Stage;
		
		private static var _instances:Dictionary = new Dictionary(true);
		private var _onStage:Point = new Point();
		
		private var eventMap:* = 
			{ mouseMove: TouchEvent.TOUCH_MOVE,
				mouseUp: TouchEvent.TOUCH_END,
				mouseDown:TouchEvent.TOUCH_BEGIN,
				click:TouchEvent.TOUCH_TAP };
	
		public function MouseToTouch(s:Stage)
		{
			if(!s) throw new ArgumentError("mouseToTouch requires a valid stage");
			
			// make sure we're only hooked up once per stage
			if(s in _instances) return;
			_instances[s] = this;
			
			_stage = s;
			_stage.addEventListener(MouseEvent.MOUSE_DOWN, onDown, true,int.MAX_VALUE);
			_stage.addEventListener(MouseEvent.CLICK, onEvent, true,int.MAX_VALUE);
		}
		
		private function onDown(e:MouseEvent):void {
			_stage.addEventListener(MouseEvent.MOUSE_MOVE, onEvent, true,int.MAX_VALUE);
			_stage.addEventListener(MouseEvent.MOUSE_UP, onUp, true,int.MAX_VALUE);
			onEvent(e);
		}
		private function onEvent(e:MouseEvent):void {
			dispatchAsTouch(e);
			e.stopImmediatePropagation();
		}
		
		private function onUp(e:MouseEvent):void {
			onEvent(e);
			_stage.removeEventListener(MouseEvent.MOUSE_MOVE, onEvent, true);
			_stage.removeEventListener(MouseEvent.MOUSE_UP, onUp, true);
		}	
		
		private var _type:String;
		private function dispatchAsTouch(e:MouseEvent):void {
			_onStage.x = e.stageX;
			_onStage.y = e.stageY;
			
			_type = eventMap[e.type];
			var targets:Array = _stage.getObjectsUnderPoint(_onStage);
			targets.reverse();
			targets.push(_stage);
			

			_stopped = false;
			
			for each (var target:DisplayObject in targets) {
				var iObj:InteractiveObject = target as InteractiveObject;
				dispatchTo(iObj);
				if(_stopped) break;
			}
		}
		
		private function dispatchTo(iTarget:InteractiveObject):Boolean {
			
			while(iTarget) {						// climb up from the hit object to an interactiveObject
				var local:Point = iTarget.globalToLocal(_onStage);
				var event:TouchEvent = new StoppableTouchEvent(_type,true,false,1, true, local.x, local.y, 0,0,1,iTarget);
			
				if(iTarget)
				{
					return iTarget.dispatchEvent(event); 
				}
				iTarget = iTarget.parent;	
			}
			return true;
		}
		
	
		public static var _stopped:Boolean;
	}
}
import flash.display.InteractiveObject;
import flash.events.TouchEvent;

import xd.touch.MouseToTouch;

class StoppableTouchEvent extends TouchEvent {
	
public function StoppableTouchEvent(type:String, bubbles:Boolean=true, cancelable:Boolean=false, touchPointID:int=0, isPrimaryTouchPoint:Boolean=false, 
						 localX:Number=NaN, localY:Number=NaN, sizeX:Number=0, sizeY:Number=0, pressure:Number=1, relatedObject:InteractiveObject=null, 
						 ctrlKey:Boolean=false, altKey:Boolean=false, shiftKey:Boolean=false, commandKey:Boolean=false, controlKey:Boolean=false,
						 timeStamp:Number=NaN, intent:String = "unknown", buttons:int=0)
{
	super(type, bubbles, cancelable, touchPointID, isPrimaryTouchPoint, localX, localY, sizeX, sizeY, pressure, relatedObject, ctrlKey, altKey, shiftKey, commandKey, controlKey,timeStamp,intent);
	
}

public override function stopImmediatePropagation():void { MouseToTouch._stopped = true; super.stopImmediatePropagation()}
public override function stopPropagation():void { MouseToTouch._stopped = true; super.stopPropagation()}
}

