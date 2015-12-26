#version 120

varying vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;

void main(void) {
  /*vec4 color = vec4(1.0,1.0,1.0,1.0);*/
  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).a) * color;
  /*gl_FragColor = vec4(1,1,1,1);*/
}
