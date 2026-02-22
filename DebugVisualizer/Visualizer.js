visualizer.width = 1600
visualizer.height = 1600
const ctx = visualizer.getContext("2d")

function clear(){
    ctx.fillStyle = "black"
    ctx.fillRect(0,0,1600,1600)
}

// from 0 to 1
function Point(xy){
    ctx.fillStyle = "white"
    ctx.fillRect(xy[0]-2,xy[1]-2,4,4)
}

function triangle(A, B, C, col){
    ctx.fillStyle = col
    ctx.beginPath()
    ctx.moveTo(A[0],A[1])
    ctx.lineTo(B[0],B[1])
    ctx.lineTo(C[0],C[1])
    ctx.closePath()
    ctx.fill()

    ctx.globalAlpha = 1
    Point(A)
    Point(B)
    Point(C)
}

function basisToScreen(A){
    return [100 + A[0]*400, 100+ A[1]*400]
}

B1 = [0,0,0]
b2 = [1,0,0]
b3 = [0,0,1]

cross = [0,1,0]

function distOffTriangle(A){
    return A[0]*cross[0] + A[1]*cross[1] + A[2]*cross[2]
}

function toBasis(A){
    A = [A[0]-B1[0], A[1]-B1[1], A[2]-B1[2]]
    d = distOffTriangle(A)
    A = [A[0] - d*cross[0] , A[1] - d*cross[1] , A[2] - d*cross[2]]

    for(i in [0,1,2]) for(j in [0,1,2]){
        if(i!=j && Math.abs(b2[i]) > 1e-20){
            if(Math.abs(b2[i]*b3[j] - b2[j]*b3[i]) > 1e-20){
                b = (b2[i]*A[j] - b2[j]*A[i])/ (b2[i]*b3[j] - b2[j]*b3[i])
                a = (A[i] - b*b3[i])/b2[i]
                return [a , b]
            }
        }
    }

    return [-1, -1]
}

function calculateCross(){
    cross = [
        b2[1]*b3[2] - b2[2]*b3[1],
        b2[2]*b3[0] - b2[0]*b3[2],
        b2[0]*b3[1] - b2[1]*b3[0]
    ]
    length = Math.sqrt(cross[0]*cross[0] + cross[1]*cross[1] + cross[2]*cross[2])
    cross [cross[0]/length, cross[1]/length, cross[2]/length]
}

function original(A, B, C){
    B1 = A
    b2 = [B[0]-A[0], B[1]-A[1], B[2]-A[2]]
    b3 = [C[0]-A[0], C[1]-A[1], C[2]-A[2]]
    clear()

    ctx.globalAlpha = 0.5
    triangle(basisToScreen([0,0]), basisToScreen([1,0]), basisToScreen([0,1]), "#0FF")

    calculateCross()
}

function checkCoplanarity(A,B,C){
    A = [A[0]-B1[0], A[1]-B1[1], A[2]-B1[2]]
    B = [B[0]-B1[0], B[1]-B1[1], B[2]-B1[2]]
    C = [C[0]-B1[0], C[1]-B1[1], C[2]-B1[2]]

    a = Math.abs(distOffTriangle(A))
    b = Math.abs(distOffTriangle(B))
    c = Math.abs(distOffTriangle(C))

    if(a>1e-20) console.debug("first  point is not coplanar with the original triangle by "+a)
    if(b>1e-20) console.debug("second point is not coplanar with the original triangle by "+b)
    if(c>1e-20) console.debug("third  point is not coplanar with the original triangle by "+c)
}

function newTriangle(col, A, B, C){
    checkCoplanarity(A,B,C);

    A = basisToScreen(toBasis(A))
    B = basisToScreen(toBasis(B))
    C = basisToScreen(toBasis(C))
    
    r = (col&4)>>2
    g = (col&2)>>1
    b = col&1
    
    rgb = "#" + ((r==1)?"ff":"00") + ((g==1)?"ff":"00")  + ((b==1)?"ff":"00")
    console.debug(rgb) 

    console.debug(A,B,C)
    
    ctx.globalAlpha = 0.5
    triangle(A, B, C, rgb)
}


clear()