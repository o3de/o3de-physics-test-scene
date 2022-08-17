-- mobile_robot_control.lua 

--[[
Proof-of-concept mobile control physics
--]]




local mobile_robot_control = 
{
    Properties =
    {
        wheel_L = { default = EntityId() },
        wheel_R = { default = EntityId() },
        steering_L = { default = EntityId() },
        steering_R = { default = EntityId() },
        speed_coef = { default = 0.2},
        steering_coef = { default = 0.2},
    }
}


function mobile_robot_control:OnActivate()
     -- Activation Code

     Debug.Log("Mobile Robot Physics initialization")

     self.tickBusHandler = TickBus.CreateHandler(self)
     self.tickBusHandler:Connect()    
     
     self.InputNotificationBus = InputEventNotificationBus.Connect(self, InputEventNotificationId("mobile_robot_keys"))
                    
     self.tickTime = 0 
     
end

function mobile_robot_control:OnHeld  (value)
    -- Process key press
    -- UP --
    if value == 1.0 then
        --Debug.Log("UP");
        local tm = TransformBus.Event.GetWorldTM(self.Properties.wheel_L)
        v = Transform.TransformVector(tm, Vector3(0,0,self.Properties.speed_coef))
        RigidBodyRequestBus.Event.ApplyAngularImpulse(self.Properties.wheel_L, v)

        local tm = TransformBus.Event.GetWorldTM(self.Properties.wheel_R)
        v = Transform.TransformVector(tm, Vector3(0,0,-self.Properties.speed_coef))
        RigidBodyRequestBus.Event.ApplyAngularImpulse(self.Properties.wheel_R, v)
    end
    -- DOWN --
    if value == 2.0 then
        --Debug.Log("DOWN");
        local tm = TransformBus.Event.GetWorldTM(self.Properties.wheel_L)
        v = Transform.TransformVector(tm, Vector3(0,0,-self.Properties.speed_coef))
        RigidBodyRequestBus.Event.ApplyAngularImpulse(self.Properties.wheel_L, v)

        local tm = TransformBus.Event.GetWorldTM(self.Properties.wheel_R)
        v = Transform.TransformVector(tm, Vector3(0,0,self.Properties.speed_coef))
        RigidBodyRequestBus.Event.ApplyAngularImpulse(self.Properties.wheel_R, v)
    end
    -- LEFT --
    if value == 3.0 then
        --Debug.Log("LEFT");
        local tm = TransformBus.Event.GetWorldTM(self.Properties.steering_R)
        v = Transform.TransformVector(tm, Vector3(0, 0, self.Properties.steering_coef))
        RigidBodyRequestBus.Event.ApplyAngularImpulse(self.Properties.steering_R, v)

        local tm = TransformBus.Event.GetWorldTM(self.Properties.steering_L)
        v = Transform.TransformVector(tm, Vector3(0, 0, self.Properties.steering_coef))
        RigidBodyRequestBus.Event.ApplyAngularImpulse(self.Properties.steering_L, v)

    end
    -- RIGHT --
    if value == 4.0 then
        --Debug.Log("RIGHT");
        local tm = TransformBus.Event.GetWorldTM(self.Properties.steering_R)
        v = Transform.TransformVector(tm, Vector3(0, 0, -self.Properties.steering_coef))
        RigidBodyRequestBus.Event.ApplyAngularImpulse(self.Properties.steering_R, v)

        local tm = TransformBus.Event.GetWorldTM(self.Properties.steering_L)
        v = Transform.TransformVector(tm, Vector3(0, 0, -self.Properties.steering_coef))
        RigidBodyRequestBus.Event.ApplyAngularImpulse(self.Properties.steering_L, v)
    end
end

function mobile_robot_control:OnDeactivate()
     -- Deactivation Code
end

-- This callback is called every frame by the tick bus after this entity activates
function mobile_robot_control:OnTick(deltaTime, timePoint)
    self.tickTime = self.tickTime + deltaTime
    -- Output some debug info (if needed), but not too often
    if self.tickTime > 0.1 then
       --local v = RigidBodyRequestBus.Event.GetAngularVelocity(self.Properties.wheel_L)
       --local tm = TransformBus.Event.GetLocalTM(self.Properties.wheel_L)
       --v = Transform.TransformVector(tm, v)
       --Debug.Log(tostring(v))
       self.tickTime = 0
    end    

end


return mobile_robot_control