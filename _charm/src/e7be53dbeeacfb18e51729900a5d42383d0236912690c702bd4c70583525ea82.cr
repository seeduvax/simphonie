<?xml version="1.0" encoding="UTF-8"?><cr id="e7be53dbeeacfb18e51729900a5d42383d0236912690c702bd4c70583525ea82" state="working">
    <title>Simulator Publish/Configure/Connect implementation not complient to SMP requirement</title>
    <reporter>sdevaux</reporter>
    <creation>2024-03-14 12:17:00+01:00</creation>
    <description>
        <p>When publishing/configure/connect of a component, the simulator shall iterate</p>
        <p>on sub component.</p>
        <p>When configure a component, the simulator shall publish it if not done yet.</p>
        <p>When connect a component, the simulatator shall configure it if not done yet.</p>
        <p>See details from ECSS-E-ST-40-07C document (march 2020 edition), Â§5.3.7</p>
    </description>
    <links>
        <link name="parent"> 15f134bf873c39117f7784713652054388364d9e207768dd48bd6a51cbac1fbc</link>
    </links>
    <cf v="sdevaux 2024-03-16T18:57:46+01:00"/>
</cr>
