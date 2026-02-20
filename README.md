# NanoVR
BP_WalkieTalkie

1. C++ Class - WalkieTalkieActor.cpp and WalkieTalkieActor.h

WalkieTalkie Actor (C++):
Static Mesh: Visual 3D walkie talkie.
Sphere Component (no mesh, collision only): Detection boundary around the walkie talkie.
3D Widget Component: “Press E” prompt floating above walkie talkie (hidden by default).
Overlap events enable/disable input interaction.
On E key press, show a 2D chat input UI on screen.

Components:
UStaticMeshComponent* WalkieTalkieMesh; (assign your static mesh in Editor).
USphereComponent* DetectionSphere; (collision only, no mesh).
UWidgetComponent* PressEPromptWidget; (3D widget displaying “Press E”).

Logic:
Bind OnComponentBeginOverlap & OnComponentEndOverlap on DetectionSphere to detect player presence.
Show/hide PressEPromptWidget accordingly.
Enable/disable input for the player controller when inside detection.
Bind to E key press:
Hide the 3D prompt.
Show the 2D chat input widget on screen.

Note: 3D widget (PressEPromptWidget) is a world-space widget attached above the walkie talkie mesh.

2. Blueprint inheriting from Walkie Talkie Actor C++ class - BP_WalkieTalkie, PressEPromptWidget UI widget class assigned to WBP_PressE, In levelinstance, UI Chat Input Widget Class set to WBP_ChatInput1.
3. Depreciated Add Input Action Mapping for “Interact” (E key) (Action Mappings, named 'Interact', Bind 'E' keyboard key
4. WBP_PressE - Button -> Text (Press E to Interact)
5. WBP_ChatInput - Canvas Panel -> Border -> Editable Text named InputTextBox
   Event on InputTextBox: OnTextCommitted (Variables: Target - WBP Chat Input, InputTextBox - Editable Text Box Widget, Event Dispatchers: OnTextSubmitted)


Current Step:
- Get WBP_ChatInput1 OnTextSubmitted event dispatcher working and C++ code working
- 
In C++:
Hold a TSubclassOf<UUserWidget> ChatInputWidgetClass property.
Create an instance (UUserWidget* ChatInputWidget) at runtime on demand.
Add to viewport, set input mode UI only, and show cursor.
On text submit:
Pass the string input to WalkieTalkie actor method.

Future Steps:
-  HTTP POST Request for Chat Message
Use UE’s FHttpModule for asynchronous requests.
Prepare JSON body: { "message": "<YourUserText>", "max_tokens": 512, "temperature": 0.7 }.
Set headers for Content-Type: application/json.
Send to http://<your_api_address>:8000/chat.
Bind to completion response delegate.
Parse JSON reply (response field) with FJsonSerializer.
Forward AI response to UI or log.

- Mode Selection Logic
After pressing E, on the 3D widget hiding, display a mode select UI (can be a simple widget with options).
Based on user choice, enable the chat input widget.
UI done as a UMG widget with buttons, selected and handled in C++.




 
