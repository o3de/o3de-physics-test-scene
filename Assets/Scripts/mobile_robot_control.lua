-- mobile_robot_control.lua 

--[[
    This is a proof-of-concept implementation of vehicle control physics. 
    It allows controlling the speed and steering angle of the vehicle 
    using the keyboard.
--]]


local mobile_robot_control = 
{
    Properties =
    {
        -- Entities that are needed by the control
        entities = 
        {
            wheelLR = { default = EntityId() },
            wheelRR = { default = EntityId() },
            wheelLF = { default = EntityId() },
            wheelRF = { default = EntityId() },
            steeringL = { default = EntityId() },
            steeringR = { default = EntityId() },
            payload = { default = EntityId() },
        },
        maxSteeringAngle = { 
            default = 0.5,
            suffix = "rad",
            description = "The maximal angle of the steering"
        },
        maxSpeed = { 
            default = 2.0,
            suffix = "m/s",  
            description = "The maximal speed"
        },
        frontWheelsDrive = { 
            default = false,
            description = "If set, front weels will be driven"
        },
        rearWheelsDrive = { 
            default = true,
            description = "If set, rear weels will be driven"
        },
    }
}


function mobile_robot_control:OnActivate()
     -- Activation Code

     Debug.Log("Mobile Robot Physics initialization")

     self.tickBusHandler = TickBus.CreateHandler(self)
     self.tickBusHandler:Connect()    
     
     self.InputNotificationBus = InputEventNotificationBus.Connect(self, InputEventNotificationId("mobile_robot_keys"))

     -- These vars indicates if arrow keys are pressed
     self.steeringWheelActive = false
     self.speedControlWheelActive = false

     -- Values of speed and steering we want to maintain
     self.targetSteeringAngle = 0.0     
     self.targetSpeed = 0.0
end

function mobile_robot_control:_SetWheelSteeringAngle(entityid, targetAngle)
    -- Applies torque to the steering knuckle trying to achieve targetAngle

    -- TODO make the condition below working
    --if not entityid==nil then

        -- Obtaining current rotation of steering knuckle
        local knuckleRotation = TransformBus.Event.GetLocalRotation(entityid)['z']
        -- Calculate difference between target and current angles
        local dif = targetAngle - knuckleRotation
        -- Calculate torque to be applied
        local torque = dif*10.0
        -- We want to have more effect when dif is larger
        torque = torque*torque*torque

        -- Obtain transformation of knuckle
        local tm = TransformBus.Event.GetWorldTM(entityid)
        -- ...and use it to create torque vector
        local torqueVector = Transform.TransformVector(tm, Vector3(0, 0, torque))

        -- Apply this vector to the knuckle
        RigidBodyRequestBus.Event.ApplyAngularImpulse(entityid, torqueVector)
    --else
    --    Debug.Error("Steering objects not set")
    --end
end


function mobile_robot_control:SetSteering(targetAngle) -- rad
    -- Rotates both steering knuckles to achieve targetAngle
    self:_SetWheelSteeringAngle(self.Properties.entities.steeringR, targetAngle)  
    self:_SetWheelSteeringAngle(self.Properties.entities.steeringL, targetAngle)  
end


function mobile_robot_control:_SetSpeedOneWheel(entityid, torque)
    -- Apply torque to one wheel
    local tm = TransformBus.Event.GetWorldTM(entityid)
    local torqueVector = Transform.TransformVector(tm, Vector3(0,0,torque))
    RigidBodyRequestBus.Event.ApplyAngularImpulse(entityid, torqueVector)
end


function mobile_robot_control:SetSpeed(targetSpeed, accelerationCoefficient)
    -- Applies torque to driven wheels, trying to maintain targetSpeed

    local tm = TransformBus.Event.GetWorldTM(self.entityId)
    Transform.Invert(tm)
    local v = RigidBodyRequestBus.Event.GetLinearVelocity(self.entityId)
    v = Transform.TransformVector(tm, v)
    local currentSpeed = v["y"]

    dif = targetSpeed - currentSpeed
    torque = dif * accelerationCoefficient
    if torque>0 then 
        torque = torque +0.1
    elseif torque<0 then
        torque = torque -0.1
    end


    if self.Properties.rearWheelsDrive then
        self:_SetSpeedOneWheel(self.Properties.entities.wheelLR, torque)
        self:_SetSpeedOneWheel(self.Properties.entities.wheelRR, -torque)
    end

    if self.Properties.frontWheelsDrive then
        self:_SetSpeedOneWheel(self.Properties.entities.wheelLF, torque)
        self:_SetSpeedOneWheel(self.Properties.entities.wheelRF, -torque)
    end
end


function mobile_robot_control:OnHeld  (value)
    -- Keypress actions

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
        local tm = TransformBus.Event.GetWorldTM(self.Properties.entities.payload)
        v = Transform.TransformVector(tm, Vector3(7,0, 20))
        RigidBodyRequestBus.Event.ApplyLinearImpulse(self.Properties.entities.payload, v)
        v = Transform.TransformVector(tm, Vector3(0,0.5, 0))
        RigidBodyRequestBus.Event.ApplyAngularImpulse(self.Properties.entities.payload, v)
    end
end

function mobile_robot_control:OnDeactivate()
     -- Deactivation Code
end

-- This callback is called every frame by the tick bus after this entity activates
function mobile_robot_control:OnTick(deltaTime, timePoint)


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
    --Debug.Log("v="..tostring(currentSpeed))

end

return mobile_robot_control