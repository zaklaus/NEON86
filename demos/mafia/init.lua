local mapModel, colsModel
local camera

local cam = require "camera"

local bgColor = VectorRGBA(42, 16, 9)

local cols = require "collisions"

local world = cols.newWorld()
local lights = {}

-- Create a sunlight
sun = Light()
sun:setType(LIGHTKIND_DIRECTIONAL)
sun:setDirection(Vector(-1,-1,1))
sun:setDiffuse(Color(32, 46, 90))
sun:enable(true, 0)

-- Register local font and create font drawer
RegisterFontFile("slkscr.ttf")
testFont = Font("Silkscreen", 16, 700, false)

-- Initialize camera
camera = cam.newCamera(Vector3( -39.431,3.21591,1.2078))
camera.angles = { 0.60000003129245, 0.097500005085019}
camera.grounded = false

function _init()
  mapModel = Model("map.fbx", true, false)
  
  for _, meshNode in pairs(mapModel:getRootNode():getNodes()) do
    for _, mesh in pairs(meshNode:getMeshes()) do
      for _, part in pairs(mesh:getParts()) do
        slMesh = cols.newTriangleMeshFromPart(part, meshNode:getFinalTransform())
        slMesh.friction = 1.0
        world:addCollision(slMesh)
      end
    end
  end
  camera.updateMovement = function(self, dt)
    self.vel = self.movedir
    world:forEach(function (shape)
      shape:testSphere(self.pos, 0.5, self.movedir, function (norm)
        self.vel = cols.slide(self.movedir, norm)
        self.grounded = true
      end)
    end)

    self.pos = self.pos + self.vel
  end

  idx = 1
  for a, lv in pairs(mapModel:getRootNode():findNode("lights"):getTargets()) do
    l = Light()
    l:setType(LIGHTKIND_POINT)
    l:setPosition(lv:row(4))
    l:setDiffuse(206, 79, 47)
    l:setSpecular(0.12,0.12,0.12,1)
    l:setRange(50)
    l:setAttenuation(0,0.06,0)
    l:enable(true, idx)
    table.insert(lights, l)
    idx = idx
  end

  EnableLighting(true)
  SetFog(bgColor, FOGKIND_EXP, 0.008)

  ShowCursor(false)
  SetCursorMode(CURSORMODE_CENTERED)
end

function _update(dt)
  if GetKeyDown(KEY_ESCAPE) then
      ExitGame()
  end

  if GetKeyDown(KEY_F2) then
    ShowCursor(not IsCursorVisible())
    SetCursorMode(1-GetCursorMode())
  end

  if GetKeyDown(KEY_F3) then
      LogString("campos: " .. vec2str(camera.pos) .. "\n")
      LogString("camangle: " .. camera.angles[1] .. ", " .. camera.angles[2] .. "\n")
  end

  camera:update(dt)
  time = getTime()
  camera.grounded = false
end

function _render()
  ClearScene(bgColor:color())
  AmbientColor(bgColor:color())

  for i, l in pairs(lights) do
    l:enable(true, i)
  end

  camera.mat:bind(VIEW)
  CameraPerspective(62, 0.1, 9000)
  mapModel:draw(Matrix())
end

function _render2d()
  testFont:drawText(0xFFFFFFFF,[[

Tristram map viewer

Map made by Cukier for Mafia: The City of Lost Heaven.

  ]], 15, 30, 1200, 800)

      testFont:drawText(Color(255,64,0), "Move with WASD", 15, 600)
end