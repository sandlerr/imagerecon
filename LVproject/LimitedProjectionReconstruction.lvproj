<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="13008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="Clusters" Type="Folder">
			<Item Name="backproject_input.ctl" Type="VI" URL="../backproject_input.ctl"/>
			<Item Name="bp_work_unit.ctl" Type="VI" URL="../bp_work_unit.ctl"/>
			<Item Name="fp_comp_input.ctl" Type="VI" URL="../fp_comp_input.ctl"/>
			<Item Name="pixels_for_proj.ctl" Type="VI" URL="../pixels_for_proj.ctl"/>
			<Item Name="tags_cluster.ctl" Type="VI" URL="../tags_cluster.ctl"/>
		</Item>
		<Item Name="Sub VIs" Type="Folder">
			<Item Name="Backproject" Type="Folder">
				<Item Name="backproject-master.vi" Type="VI" URL="../backproject-master.vi"/>
				<Item Name="backproject-normalise.vi" Type="VI" URL="../backproject-normalise.vi"/>
				<Item Name="backproject-worker.vi" Type="VI" URL="../backproject-worker.vi"/>
				<Item Name="backproject.vi" Type="VI" URL="../backproject.vi"/>
			</Item>
			<Item Name="FBP" Type="Folder">
				<Item Name="fbp-worker.vi" Type="VI" URL="../fbp-worker.vi"/>
				<Item Name="fbp.vi" Type="VI" URL="../fbp.vi"/>
			</Item>
			<Item Name="Initialisation" Type="Folder">
				<Item Name="GetAngles.vi" Type="VI" URL="../GetAngles.vi"/>
				<Item Name="openTiffImage.vi" Type="VI" URL="../openTiffImage.vi"/>
				<Item Name="ReadTextUntilError.vi" Type="VI" URL="../ReadTextUntilError.vi"/>
				<Item Name="tiffTags.vi" Type="VI" URL="../tiffTags.vi"/>
				<Item Name="tiffVersion.vi" Type="VI" URL="../tiffVersion.vi"/>
			</Item>
			<Item Name="utility" Type="Folder">
				<Item Name="createOutputFile.vi" Type="VI" URL="../createOutputFile.vi"/>
				<Item Name="queue-cleanup.vi" Type="VI" URL="../queue-cleanup.vi"/>
				<Item Name="scale to u8.vi" Type="VI" URL="../scale to u8.vi"/>
			</Item>
			<Item Name="art_pass.vi" Type="VI" URL="../art_pass.vi"/>
			<Item Name="forward proj comparison.vi" Type="VI" URL="../forward proj comparison.vi"/>
			<Item Name="laplacian.vi" Type="VI" URL="../laplacian.vi"/>
			<Item Name="readImage.vi" Type="VI" URL="../readImage.vi"/>
		</Item>
		<Item Name="Main.vi" Type="VI" URL="../Main.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Clear Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Clear Errors.vi"/>
				<Item Name="NI_AALPro.lvlib" Type="Library" URL="/&lt;vilib&gt;/Analysis/NI_AALPro.lvlib"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
			</Item>
			<Item Name="labviewdll.dll" Type="Document" URL="../../imagereconstruction/x64/Debug/labviewdll.dll"/>
			<Item Name="lvanlys.dll" Type="Document" URL="/&lt;resource&gt;/lvanlys.dll"/>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="Main" Type="EXE">
				<Property Name="App_copyErrors" Type="Bool">true</Property>
				<Property Name="App_INI_aliasGUID" Type="Str">{35B9F610-67F7-43B0-8274-5FC2780275C5}</Property>
				<Property Name="App_INI_GUID" Type="Str">{3B42B4D4-322F-4D04-ABD9-31587D19C622}</Property>
				<Property Name="App_serverConfig.httpPort" Type="Int">8002</Property>
				<Property Name="Bld_autoIncrement" Type="Bool">true</Property>
				<Property Name="Bld_buildCacheID" Type="Str">{EDC2BC28-EDC7-4416-A055-8A0FCD647F93}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">Main</Property>
				<Property Name="Bld_excludeInlineSubVIs" Type="Bool">true</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_localDestDir" Type="Path">../builds/NI_AB_PROJECTNAME/Main</Property>
				<Property Name="Bld_localDestDirType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{5D66724C-C351-456E-9068-483949DEC431}</Property>
				<Property Name="Bld_version.build" Type="Int">1</Property>
				<Property Name="Bld_version.major" Type="Int">1</Property>
				<Property Name="Destination[0].destName" Type="Str">Application.exe</Property>
				<Property Name="Destination[0].path" Type="Path">../builds/NI_AB_PROJECTNAME/Main/Application.exe</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">../builds/NI_AB_PROJECTNAME/Main/data</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="Source[0].itemID" Type="Str">{64B8791E-65FE-456E-8E0D-8E0DF6199D53}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/Main.vi</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">VI</Property>
				<Property Name="SourceCount" Type="Int">2</Property>
				<Property Name="TgtF_companyName" Type="Str">R C &amp; A</Property>
				<Property Name="TgtF_fileDescription" Type="Str">Main</Property>
				<Property Name="TgtF_internalName" Type="Str">Main</Property>
				<Property Name="TgtF_legalCopyright" Type="Str">Copyright © 2014 R C &amp; A</Property>
				<Property Name="TgtF_productName" Type="Str">Main</Property>
				<Property Name="TgtF_targetfileGUID" Type="Str">{906742F8-F7A6-41DA-A065-376A53B695FA}</Property>
				<Property Name="TgtF_targetfileName" Type="Str">Application.exe</Property>
			</Item>
		</Item>
	</Item>
</Project>
