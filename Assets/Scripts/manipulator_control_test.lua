local manipulator_control = 
{
    Properties =
    {
        -- Entities that are needed by the control
        entities = 
        {
            sliderX = { default = EntityId() },
            sliderY = { default = EntityId() },
            sliderZ = { default = EntityId() },
        },
    }
}


--[[
    PID controller
    Credits: https://steamcommunity.com/sharedfiles/filedetails/?id=1800568163
]]
function pid(p,i,d)
    return{p=p,i=i,d=d,E=0,D=0,I=0,
        run=function(s,sp,pv)
            local E,D,A
            E = sp-pv
            D = E-s.E
            A = math.abs(D-s.D)
            s.E = E
            s.D = D
            s.I = A<E and s.I +E*s.i or s.I*0.5
            return E*s.p +(A<E and s.I or 0) +D*s.d
        end
    }
end

-- Creating PID controllers for each DOF
pidX = pid(10.0, 0.1, 200)
pidZ = pid(10.0, 0.0, 200)
pidY = pid(0.001, 10.0, 200)


--[[
    Check and return position of slider 'entityid'
]]
function manipulator_control:GetSliderPosition(entityid)
    if entityid~=nil then
        local vec = TransformBus.Event.GetLocalTranslation(entityid)
        if vec ~= nil then
            return vec['x']
        else
            return nil
        end
    else
        return nil
    end
end



function manipulator_control:OnActivate()
     -- Activation Code

     -- To process ticks
     self.tickBusHandler = TickBus.CreateHandler(self)
     self.tickBusHandler:Connect()    
     
     -- and inputs
     self.InputNotificationBus = InputEventNotificationBus.Connect(self, InputEventNotificationId("mobile_robot_keys"))

     -- For debugging output
     self.timeSinceBigTick = 0

     -- Range of X and Z sliders
     self.sliderRangeX = {0.7, 2.3}
     self.sliderRangeZ = {0.7, 2.3}

     -- Tables with positions to be used when moving sliders with arrows
     self.positionTableX = {self.sliderRangeX[1], (self.sliderRangeX[2]+self.sliderRangeX[1])/2, self.sliderRangeX[2]}
     self.positionTableIdxX = 1 -- index of current target position
     self.positionTableZ = {self.sliderRangeZ[1], (self.sliderRangeZ[2]+self.sliderRangeZ[1])/2, self.sliderRangeZ[2]}
     self.positionTableIdxZ = 1 -- index of current target position

     -- Initial target positions
     self.sliderPosX = self.positionTableX[self.positionTableIdxX]
     self.sliderPosZ = self.positionTableZ[self.positionTableIdxZ]

     -- The same for the Y slider will be set in one of first ticks. It is
     -- based on current position, which may be nil right now.
     self.sliderRangeY = nil     
     self.positionTableY = {nil, nil}
     self.positionTableIdxY = 1
     self.sliderPosY = nil
end



function manipulator_control:OnDeactivate()
     -- Deactivation Code
end


--[[
    Apply a vfoece impulse 'value' to the slider defined by 'entityid'
]]
function manipulator_control:MoveSlider(entityid, value)
    local tm = TransformBus.Event.GetWorldTM(entityid)
    local forceVector = Transform.TransformVector(tm, Vector3(value,0,0))
    RigidBodyRequestBus.Event.ApplyLinearImpulse(entityid, forceVector)
end

--[[
    Move the slider X
    Get a value of force from the PID and apply it to the slider
    calling 'MoveSlider'
]]
function manipulator_control:MoveSliderX(deltaTime)
    -- Sleder entity
    local entityid = self.Properties.entities.sliderX
    -- Current position
    local currentSliderPos = self:GetSliderPosition(self.Properties.entities.sliderX)
    if currentSliderPos ~= nil and self.sliderPosX ~= nil then -- sanity check
        -- Assess force using PID
        local force = pidX:run(self.sliderPosX, currentSliderPos)*deltaTime*10.0
        -- Apply it to the slider
        self:MoveSlider(entityid, force)
    end
end


--[[
    Move the slider Z
    Get a value of force from the PID and apply it to the slider
    calling 'MoveSlider'
]]
function manipulator_control:MoveSliderZ(deltaTime)
    -- Sleder entity
    local entityid = self.Properties.entities.sliderZ
    -- Current position
    local currentSliderPos = self:GetSliderPosition(self.Properties.entities.sliderZ)
    if currentSliderPos ~= nil and self.sliderPosZ ~= nil then -- sanity check
        -- Assess force using PID
        local force = pidZ:run(self.sliderPosZ, currentSliderPos)*deltaTime*10.0 
        -- Apply it to the slider
        self:MoveSlider(entityid, force)
    end
end


--[[
    Move the slider Y
    Get a value of force from the PID and apply it to the slider
    calling 'MoveSlider'
]]
function manipulator_control:MoveSliderY(deltaTime)
    -- Sleder entity
    local entityid = self.Properties.entities.sliderY
    -- Current position
    local currentSliderPos = self:GetSliderPosition(self.Properties.entities.sliderY)
    if currentSliderPos ~= nil and self.sliderPosY ~= nil then -- sanity check
        -- Assess force using PID
        local force = pidY:run(self.sliderPosY, currentSliderPos)*deltaTime*10.0 
        force = force * 0.10    -- For unknown reason PID output is too
                                -- large (no matter the values of p, i and d)
                                -- We must scale it down here 
        -- Apply it to the slider
        self:MoveSlider(entityid, force)
    end
end



-- function manipulator_control:OnHeld  (value)
function manipulator_control:OnPressed (value)
    -- Keypress actions

    -- Set sliders position when a key is pressed. Positions are set
    -- for each sliders basing on 'positionTables'.

    -- UP --
    if value == 1.0 then
        if self.positionTableIdxZ<3 then
            self.positionTableIdxZ = self.positionTableIdxZ + 1
        else
            self.positionTableIdxZ = 1
        end
        self.sliderPosZ = self.positionTableZ[self.positionTableIdxZ]
    end

    -- DOWN --
    if value == 2.0 then
        if self.positionTableIdxZ>1 then
            self.positionTableIdxZ = self.positionTableIdxZ - 1
        else
            self.positionTableIdxZ = 3
        end
        self.sliderPosZ = self.positionTableZ[self.positionTableIdxZ]
    end

    -- LEFT --
    if value == 3.0 then
        if self.positionTableIdxX>1 then
            self.positionTableIdxX = self.positionTableIdxX - 1
        else
            self.positionTableIdxX = 3
        end

        self.sliderPosX = self.positionTableX[self.positionTableIdxX]
    end

    -- RIGHT --
    if value == 4.0 then
        if self.positionTableIdxX<3 then
            self.positionTableIdxX = self.positionTableIdxX + 1
        else
            self.positionTableIdxX = 1
        end

        self.sliderPosX = self.positionTableX[self.positionTableIdxX]
    end

    -- SPACE
    if value == 5.0 then

        if self.positionTableIdxY<2 then
            self.positionTableIdxY = self.positionTableIdxY + 1
        else
            self.positionTableIdxY = 1
        end
        self.sliderPosY = self.positionTableY[self.positionTableIdxY]
    end

    if value >= 1.0 and value <= 5.0 then
        Debug.Log("target sliders position - X: "..string.format("%1.2f",self.sliderPosX).." Z: "..
            string.format("%1.2f",self.sliderPosZ).." Y: "..string.format("%1.2f",self.sliderPosY))
    end

    -- ENTER
    if value == 6.0 then

    end
end


-- This callback is called every frame by the tick bus after this entity activates
function manipulator_control:OnTick(deltaTime, timePoint)

    self.timeSinceBigTick = self.timeSinceBigTick + deltaTime
    if self.timeSinceBigTick>.5 then
        self.timeSinceBigTick = 0

        -- Current position of sliders (for debugging purposes only)
        local sliderX_pos = self:GetSliderPosition(self.Properties.entities.sliderX)
        local sliderZ_pos = self:GetSliderPosition(self.Properties.entities.sliderZ)
        local sliderY_pos = self:GetSliderPosition(self.Properties.entities.sliderY)

        -- Print some debugging info
        if self.sliderRangeY ~= nil then
            --Debug.Log("Y: "..string.format("%1.2f",sliderY_pos).."   "..string.format("%1.2f",self.sliderPosY))
        end
    end

    -- Moving sliders - one timestep
    self:MoveSliderX(deltaTime)
    self:MoveSliderZ(deltaTime)

    --- If slider Y was initiated, read its position and set ranges and other parameters
    if self.sliderRangeY == nil then
         local sliderCurrentPosY = self:GetSliderPosition(self.Properties.entities.sliderY)
         if sliderCurrentPosY ~= nil then
             self.sliderRangeY = {sliderCurrentPosY-0.8,sliderCurrentPosY-0.2}     
             self.positionTableY = {self.sliderRangeY[1], self.sliderRangeY[2]}
             self.positionTableIdxY = 1
             --self.sliderPosY = self.positionTableY[self.positionTableIdxY]
             self.sliderPosY = (self.sliderRangeY[1]+self.sliderRangeY[2])/2
             --Debug.Log("Y range: "..tostring(self.sliderRangeY[1]).." "..tostring(self.sliderRangeY[2]).." target pos: "..tostring(self.sliderPosY))
         end
    else
        -- Moving Y slider - one timestep
        self:MoveSliderY(deltaTime)
    end
end

return manipulator_control
