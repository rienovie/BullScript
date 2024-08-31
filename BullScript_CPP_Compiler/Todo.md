# Current rough idea of stuff to focus on

# Basm compile

I think this'll be the easiest / shortest thing to start on, and I'll be able to build outward from here.

Steps for Basm compiler:
  [x]Build Bricks
    [x]Parse and translate top level to basic brick data
      Brick contains:
        -Keyword
          -Define
            Container mostly for functions
          -Create
            Values/Variables
          -Other
            Keyword that is defined with define
        -Name
          This is how the brick would be called
        -Attributes
          Usually input but will probably have other uses
        -Contents
          What is actually inside the brick
    [x]Make sure file has both entry and exit defined
    []Verify syntax
      (Will do later, possibly will verify entire script in a later step)
  []Define Bricks

  []Translate

  []Define Program

  []Build Program


Goal:
  Focus on simplifing Asm instructions with an idea of how things will integrate later.

