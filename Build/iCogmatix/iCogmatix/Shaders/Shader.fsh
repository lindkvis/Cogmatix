//
//  Shader.fsh
//  iCogmatix
//
//  Created by Gaute Lindkvist on 01/03/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
