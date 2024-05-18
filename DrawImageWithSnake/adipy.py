#!/usr/bin/env python

# https://codegolf.stackexchange.com/questions/34962/draw-an-image-with-a-snake
# https://codegolf.stackexchange.com/a/35962/11791
# User @adipy, August–October 2014

from PIL import Image  # pip install pillow
import heapq
import math
import sys

tolerance = 0.01

# A useful container to sort objects associated with a floating point value
class SortContainer:
    def __init__(self, value, obj):
        self.fvalue = float(value)
        self.obj = obj
    def __float__(self):
        return float(self.fvalue)
    def __lt__(self, other):
        return self.fvalue < float(other)
    def __eq__(self, other):
        return self.fvalue == float(other)
    def __gt__(self, other):
        return self.fvalue > float(other)

# Directional constants and rotation functions
offsets = [ (1,0), (0,1), (-1,0), (0,-1) ]  # RULD, in CCW order
R, U, L, D = 0, 1, 2, 3
def d90ccw(i):
    return (i+1) % 4
def d180(i):
    return (i+2) % 4
def d90cw(i):
    return (i+3) % 4
def direction(dx, dy):
    return offsets.index((dx,dy))


# Standard color metric: Euclidean distance in the RGB cube. Distance between opposite corners normalized to 1.
pixelMax = 255
cChannels = 3
def colorMetric(p):
    return math.sqrt(sum([ p[i]**2 for i in range(cChannels)])/cChannels)/pixelMax
def colorDistance(p1,p2):
    return colorMetric( [ p1[i]-p2[i] for i in range(cChannels) ] )


# Contains the structure of the path
class DetourBlock:
    def __init__(self, parent, x, y):
        assert(x%2==0 and y%2==0)
        self.x = x
        self.y = y
        self.parent = None
        self.neighbors = [None, None, None, None]
    def getdir(A, B):
        dx = (B.x - A.x)//2
        dy = (B.y - A.y)//2
        return direction(dx, dy)

class ImageTracer:
    def __init__(self, imgName):

        self.imgName = imgName
        img = Image.open(imgName)
        img = img.convert(mode="RGB")       # needed for BW images
        self.srcImg = [ [ [ float(c) for c in img.getpixel( (x,y) ) ] for y in range(img.size[1]) ] for x in range(img.size[0])]
        self.srcX = img.size[0]
        self.srcY = img.size[1]

        # Set up infrastructure
        self.DetourGrid = [ [ DetourBlock(None, 2*x, 2*y) \
                    for y in range((self.srcY+1)//2)] \
                    for x in range((self.srcX+1)//2)]
        self.dgX = len(self.DetourGrid)
        self.dgY = len(self.DetourGrid[0])
        self.DetourOptions = list()    # heap!
        self.DetourStart = None
        self.initPath()

    def initPath(self):
        print("Initializing")
        if not self.srcX%2 and not self.srcY%2:
            self.AssignToPath(None, self.DetourGrid[0][0])
            self.DetourStart = self.DetourGrid[0][0]
        lastDB = None
        if self.srcX%2:     # right edge initial path
            self.DetourStart = self.DetourGrid[-1][0]
            for i in range(self.dgY):
                nextDB = self.DetourGrid[-1][i]
                self.AssignToPath(lastDB, nextDB)
                lastDB = nextDB
        if self.srcY%2:     # bottom edge initial path
            if not self.srcX%2:
                self.DetourStart = self.DetourGrid[-1][-1]
            for i in reversed(range(self.dgX-(self.srcX%2))):          # loop condition keeps the path contiguous and won't add corner again
                nextDB =  self.DetourGrid[i][-1]
                self.AssignToPath(lastDB, nextDB)
                lastDB = nextDB

    # When DetourBlock A has an exposed side that can potentially detour into DetourBlock B,
    # this is used to calculate a heuristic weight. Lower weights are better, they minimize the color distance
    # between pixels connected by the snake path
    def CostBlock(self, A, B):
        # Weight the block detour based on [connections made - connections broken]
        dx = (B.x - A.x)//2
        dy = (B.y - A.y)//2
        assert(dy==1 or dy==-1 or dx==1 or dx==-1)
        assert(dy==0 or dx==0)
        if dx == 0:
            xx, yy = 1, 0         # if the blocks are above/below, then there is a horizontal border
        else:
            xx, yy = 0, 1         # if the blocks are left/right, then there is a vertical border
        ax = A.x + (dx+1)//2
        ay = A.y + (dy+1)//2 
        bx = B.x + (1-dx)//2
        by = B.y + (1-dy)//2
        fmtImg = self.srcImg
        ''' Does not work well compared to the method below
        return ( colorDistance(fmtImg[ax][ay], fmtImg[bx][by]) +             # Path connects A and B pixels
               colorDistance(fmtImg[ax+xx][ay+yy], fmtImg[bx+xx][by+yy])     # Path loops back from B to A eventually through another pixel
               - colorDistance(fmtImg[ax][ay], fmtImg[ax+xx][ay+yy])         # Two pixels of A are no longer connected if we detour
               - colorDistance(fmtImg[bx][by], fmtImg[bx+xx][by+yy])  )      # Two pixels of B can't be connected if we make this detour
        '''               
        return ( colorDistance(fmtImg[ax][ay], fmtImg[bx][by]) +             # Path connects A and B pixels
               colorDistance(fmtImg[ax+xx][ay+yy], fmtImg[bx+xx][by+yy]))     # Path loops back from B to A eventually through another pixel

    # Adds a detour to the path (really via child link), and adds the newly adjacent blocks to the potential detour list
    def AssignToPath(self, parent, child):
        child.parent = parent
        if parent is not None:
            d = parent.getdir(child)
            parent.neighbors[d] = child
            child.neighbors[d180(d)] = parent
        for (i,j) in offsets:
            x = int(child.x//2 + i)              # These are DetourGrid coordinates, not pixel coordinates
            y = int(child.y//2 + j)
            if x < 0 or x >= self.dgX-(self.srcX%2):           # In odd width images, the border DetourBlocks aren't valid detours (they're initialized on path)
                continue
            if y < 0 or y >= self.dgY-(self.srcY%2):
                continue
            neighbor = self.DetourGrid[x][y]
            if neighbor.parent is None:
                heapq.heappush(self.DetourOptions, SortContainer(self.CostBlock(child, neighbor), (child, neighbor)) )

    def BuildDetours(self, outputFilename):
        # Create the initial path - depends on odd/even dimensions
        print("Building detours")
        dbImage = Image.new("RGB", (self.dgX, self.dgY), 0)
        # We already have our initial queue of detour choices. Make the best choice and repeat until the whole path is built.
        while len(self.DetourOptions) > 0:
            sc = heapq.heappop(self.DetourOptions)       # Pop the path choice with lowest cost
            parent, child = sc.obj
            if child.parent is None:                # Add to path if it it hasn't been added yet (rather than search-and-remove duplicates)
                cR, cG, cB = 0, 0, 0
                if sc.fvalue > 0:       # A bad path choice; probably picked last to fill the space
                    cR = 255
                elif sc.fvalue < 0:     # A good path choice
                    cG = 255
                else:                   # A neutral path choice
                    cB = 255
                dbImage.putpixel( (child.x//2,child.y//2), (cR, cG, cB) )
                self.AssignToPath(parent, child)
        dbImage.save(outputFilename)

    # Reconstructing the path was a bad idea. Countless hard-to-find bugs!
    def ReconstructSnake(self, outputFilename):
        # Build snake from the DetourBlocks.
        print("Reconstructing path")
        self.path = []
        xi,yi,d = self.DetourStart.x, self.DetourStart.y, U   # good start? Okay as long as CCW
        x,y = xi,yi
        while True:
            self.path.append((x,y))
            db = self.DetourGrid[x//2][y//2]                     # What block do we occupy?
            if db.neighbors[d90ccw(d)] is None:                  # Is there a detour on my right? (clockwise)
                x,y = x+offsets[d][0], y+offsets[d][1]        # Nope, keep going in this loop (won't cross a block boundary)
                d = d90cw(d)                                  # For "simplicity", going straight is really turning left then noticing a detour on the right
            else:
                d = d90ccw(d)                                 # There IS a detour! Make a right turn
                x,y = x+offsets[d][0], y+offsets[d][1]        # Move in that direction (will cross a block boundary)
            if (x == xi and y == yi) or x < 0 or y < 0 or x >= self.srcX or y >= self.srcY:                         # Back to the starting point! We're done!
                break
        print("Retracing path length =", len(self.path))       # should = Width * Height

        # Trace the actual snake path
        pathImage = Image.new("RGB", (self.srcX, self.srcY), 0)
        cR, cG, cB = 0,0,128
        for (x,y) in self.path:
            if x >= self.srcX or y >= self.srcY:
                break
            if pathImage.getpixel((x,y)) != (0,0,0):
                print("LOOPBACK!", x, y)
            pathImage.putpixel( (x,y), (cR, cG, cB) )
            cR = (cR + 2) % pixelMax
            if cR == 0:
                cG = (cG + 4) % pixelMax
        pathImage.save(outputFilename)

    def ColorizeSnake(self, outputFilename):
        #Simple colorization of path
        traceImage = Image.new("RGB", (self.srcX, self.srcY), 0)
        print("Colorizing path")
        color = ()
        lastcolor = self.srcImg[self.path[0][0]][self.path[0][1]]
        for i in range(len(self.path)):
            v = [ self.srcImg[self.path[i][0]][self.path[i][1]][j] - lastcolor[j] for j in range(3) ]
            magv = colorMetric(v)
            if magv == 0:       # same color
                color = lastcolor
            if magv > tolerance: # only adjust by allowed tolerance
                color = tuple([lastcolor[j] + v[j]/magv * tolerance for j in range(3)])
            else:               # can reach color within tolerance
                color = tuple([self.srcImg[self.path[i][0]][self.path[i][1]][j] for j in range(3)])
            lastcolor = color
            traceImage.putpixel( (self.path[i][0], self.path[i][1]), tuple([int(color[j]) for j in range(3)]) )
        traceImage.save(outputFilename)

if __name__ == '__main__':
    imgName = sys.argv[1]
    tolerance = float(sys.argv[2]) if len(sys.argv) > 2 else 0.01
    it = ImageTracer(imgName)
    it.BuildDetours("choices_" + imgName)
    it.ReconstructSnake("path_" + imgName)
    it.ColorizeSnake("snaked_" + imgName)
