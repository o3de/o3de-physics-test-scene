-- mobile_robot_control.lua 

--[[
Proof-of-concept mobile control physics
--]]




local mobile_robot_control = 
{
    Properties =
    {
        wheelLR = { default = EntityId() },
        wheelRR = { default = EntityId() },
        wheelLF = { default = EntityId() },
        wheelRF = { default = EntityId() },
        steeringL = { default = EntityId() },
        steeringR = { default = EntityId() },
        payload = { default = EntityId() },
        speedCoef = { default = 0.2},
        maxSteeringAngle = { 
            default = 0.5,
            suffix = "rad",
        },
        maxSpeed = { 
            default = 2.0,
        },
        AWD = { 
            default = false,
            description = "If set, all weels will be driven"
        },
    }
}


function mobile_robot_control:OnActivate()
     -- Activation Code

     Debug.Log("Mobile Robot Physics initialization")

     self.tickBusHandler = TickBus.CreateHandler(self)
     self.tickBusHandler:Connect()    
     
     self.InputNotificationBus = InputEventNotificationBus.Connect(self, InputEventNotificationId("mobile_robot_keys"))
                    
     self.tickTime = 0 
     self.lastKey = 0

     self.steeringWheelActive = false
     self.speedControlWheelActive = false

     self.targetSteeringAngle = 0.0     
     self.targetSpeed = 0.0
end

function mobile_robot_control:_SetWheelSteeringAngle(entityid, targetAngle)
    -- TODO make the condition below working
    --if not entityid==nil then

        -- Obtaining current rotation of steering knuckle
        local knuckleRotation = TransformBus.Event.GetLocalRotation(entityid)['z']
        -- Calculate difference between target and current angles
        local dif = targetAngle - knuckleRotation
        -- Calculate momentum to be applied
        local momentum = dif*10.0
        -- We want to have more effect when dif is larger
        momentum = momentum*momentum*momentum

        -- Obtain transformation of knuckle
        local tm = TransformBus.Event.GetWorldTM(entityid)
        -- ...and use it to create momentum vector
        local momentumVector = Transform.TransformVector(tm, Vector3(0, 0, momentum))

        -- Apply this vector to the knuckle
        RigidBodyRequestBus.Event.ApplyAngularImpulse(entityid, momentumVector)
    --else
    --    Debug.Error("Steering objects not set")
    --end

end


function mobile_robot_control:SetSteering(targetAngle) -- rad
    self:_SetWheelSteeringAngle(self.Properties.steeringR, targetAngle)  
    self:_SetWheelSteeringAngle(self.Properties.steeringL, targetAngle)  
end


function mobile_robot_control:_SetSpeedOneWheel(entityid, force)
    local tm = TransformBus.Event.GetWorldTM(entityid)
    local v = Transform.TransformVector(tm, Vector3(0,0,force))
    RigidBodyRequestBus.Event.ApplyAngularImpulse(entityid, v)
end


function mobile_robot_control:SetSpeed(targetSpeed, accelerationCoefficient)

    local tm = TransformBus.Event.GetWorldTM(self.entityId)
    Transform.Invert(tm)
    local v = RigidBodyRequestBus.Event.GetLinearVelocity(self.entityId)
    v = Transform.TransformVector(tm, v)
    local currentSpeed = v["y"]
    --Debug.Log(tostring(currentSpeed))

    dif = targetSpeed - currentSpeed
    force = dif * accelerationCoefficient
    if force>0 then 
        force = force +0.1
    elseif force<0 then
        force = force -0.1
    end
    Debug.Log("v="..tostring(currentSpeed).."  F="..tostring(force))

    self:_SetSpeedOneWheel(self.Properties.wheelLR, force)
    self:_SetSpeedOneWheel(self.Properties.wheelRR, -force)

    if self.Properties.AWD then
        -- Debug.Log('AWD')
        self:_SetSpeedOneWheel(self.Properties.wheelLF, force)
        self:_SetSpeedOneWheel(self.Properties.wheelRF, -force)
    end



end


function mobile_robot_control:OnHeld  (value)

    accCoeff = 0.05
    brakeCoeff = 0.5

    -- Process key press
    -- UP --
    if value == 1.0 then
        --Debug.Log("UP");
        self.speedControlWheelActive = true
        self.targetSpeed = self.Properties.maxSpeed
        self:SetSpeed(self.targetSpeed, accCoeff)

    end
    -- DOWN --
    if value == 2.0 then
        --Debug.Log("DOWN");
        self.speedControlWheelActive = true
        self.targetSpeed = -self.Properties.maxSpeed
        self:SetSpeed(self.targetSpeed, accCoeff)

    end
    -- LEFT --
    if value == 3.0 then
        self.steeringWheelActive = true
        self.targetSteeringAngle = self.Properties.maxSteeringAngle

    end
    -- RIGHT --
    if value == 4.0 then
        self.steeringWheelActive = true
        self.targetSteeringAngle = -self.Properties.maxSteeringAngle
    end

    -- SPACE
    if value == 5.0 then
        self.targetSpeed = 0
        self:SetSpeed(self.targetSpeed, brakeCoeff)
    end

    -- ENTER
    if value == 6.0 then
        local tm = TransformBus.Event.GetWorldTM(self.Properties.payload)
        v = Transform.TransformVector(tm, Vector3(7,0, 20))
        RigidBodyRequestBus.Event.ApplyLinearImpulse(self.Properties.payload, v)
        v = Transform.TransformVector(tm, Vector3(0,0.5, 0))
        RigidBodyRequestBus.Event.ApplyAngularImpulse(self.Properties.payload, v)
    end

    self.lastKey = value

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

       -- local v = TransformBus.Event.GetLocalRotation(self.Properties.steering_L)
       -- Debug.Log(tostring(v))


       self.tickTime = 0
    end    

   if self.steeringWheelActive then
        self.steeringWheelActive = false
   else
        -- We want to make the return to 0 smoother
        self.targetSteeringAngle = self.targetSteeringAngle*0.91
   end 

   if self.speedControlWheelActive then
        self.speedControlWheelActive = false
   else
        self.targetSpeed = 0
   end 


   self:SetSteering(self.targetSteeringAngle)
   --self:SetSpeed(self.targetSpeed)


    local tm = TransformBus.Event.GetWorldTM(self.entityId)
    Transform.Invert(tm)
    local v = RigidBodyRequestBus.Event.GetLinearVelocity(self.entityId)
    v = Transform.TransformVector(tm, v)
    local currentSpeed = v["y"]
    Debug.Log("v="..tostring(currentSpeed))



end


return mobile_robot_control